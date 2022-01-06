unit u_RoutingDemo;

interface

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}

uses
  libosmscout_route;

procedure PrintRoute(
  const AProfile: TRouteProfile;
  const AStartPoint: point_t;
  const ATargetPoint: point_t;
  const ADatabases: array of AnsiString
);

procedure PrintLibOsmScoutVersion;

implementation

uses
  Math,
  SysUtils;

procedure PrintPoint(const APoint: ppoint_t); inline;
var
  VFormatSettings: TFormatSettings;
begin
  VFormatSettings.DecimalSeparator := '.';
  Writeln(
    FormatFloat('0.000000', APoint.lat, VFormatSettings),
    ', ',
    FormatFloat('0.000000', APoint.lon, VFormatSettings)
  );
end;

procedure PrintError(const ACtx: Pointer; const AMsg: string = '');
var
  VErr: AnsiString;
begin
  VErr := router.get_error_message(ACtx);
  Writeln(VErr, AMsg);
end;

{$IFNDEF FPC}
  {$IF CompilerVersion >= 33}
    {$DEFINE USE_NEW_EXCEPTION_MASK}
  {$IFEND}
{$ENDIF}

procedure PrintRoute(
  const AProfile: TRouteProfile;
  const AStartPoint: point_t;
  const ATargetPoint: point_t;
  const ADataBases: array of AnsiString
);
var
  I: Integer;
  VCtx: Pointer;
  VCount: uint32_t;
  VPoints: ppoint_t;
  VCalcResult: TRouteCalcResult;
  {$IFDEF USE_NEW_EXCEPTION_MASK}
  VExceptionMask: TArithmeticExceptionMask;
  {$ELSE}
  VExceptionMask: TFPUExceptionMask;
  {$ENDIF}
  VDataBasesArr: array of PAnsiChar;
begin
  LibOsmScoutRouteInitialize;

  VExceptionMask := Math.SetExceptionMask(
    [exInvalidOp, exDenormalized, exUnderflow, exPrecision]
  );

  VCtx := nil;
  try
    VCount := Length(ADataBases);
    if VCount = 0 then begin
      raise Exception.Create('Expected at least one DataBase path!');
    end;

    if VCount = 1 then begin
      if not router.new(VCtx, PAnsiChar(ADataBases[0])) then begin
        RiseLibOsmScoutError(VCtx, 'new');
      end;
    end else begin
      SetLength(VDataBasesArr, VCount);
      for I := 0 to VCount - 1 do begin
        VDataBasesArr[I] := PAnsiChar(ADataBases[I]);
      end;
      if not router.new_multi(VCtx, @VDataBasesArr[0], VCount) then begin
        RiseLibOsmScoutError(VCtx, 'new_multi');
      end;
    end;

    VCalcResult := router.calc(VCtx, AProfile, @AStartPoint,
      @ATargetPoint, VCount, VPoints);

    case VCalcResult of

      CALC_RESULT_OK: begin
        if VCount = 0 then begin
          PrintError(VCtx, 'There is no points in route!');
        end else begin
          for I := 0 to VCount - 1 do begin
            PrintPoint(VPoints);
            Inc(VPoints);
          end;
        end;

        router.clear(VCtx);
      end;

      CALC_RESULT_NODATA_START: begin
        PrintError(VCtx, 'There is no data in database for start location!');
      end;

      CALC_RESULT_NODATA_TARGET: begin
        PrintError(VCtx, 'There is no data in database for target location!');
      end;

      CALC_RESULT_NODATA_ROUTE: begin
        PrintError(VCtx, 'The route cannot be built for the given profile. Not enough data in the database!');
      end;

      CALC_RESULT_ERROR: begin
        RiseLibOsmScoutError(VCtx, 'calc');
      end;
    else
      raise Exception.CreateFmt(
        '"router_calc" returns unexpected result: %d', [Integer(VCalcResult)]
      );
    end;
  finally
    Math.SetExceptionMask(VExceptionMask);
    router.del(VCtx);
  end;
end;

procedure PrintLibOsmScoutVersion;
var
  VInfo: prouter_version_t;
begin
  LibOsmScoutRouteInitialize;

  VInfo := router.get_version;
  Writeln(
    'Built with libosmscout-', VInfo.libosmscout_commit_hash,
    ' (DB file format version: ', VInfo.libosmscout_db_file_version, ')'
  );
end;

end.
