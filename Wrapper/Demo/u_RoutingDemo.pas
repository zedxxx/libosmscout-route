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
  VOpt: Pointer;
  VCtx: Pointer;
  VCount: uint32_t;
  VPoints: ppoint_t;
  VResult: TRouterResult;
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

  VOpt := nil;
  VCtx := nil;
  try
    VCount := Length(ADataBases);
    if VCount = 0 then begin
      raise Exception.Create('Expected at least one DataBase path!');
    end;

    VResult := router.opt_new(VOpt);
    if VResult <> ROUTER_RESULT_OK then begin
      RiseLibOsmScoutError(nil, 'opt_new');
    end;

    SetLength(VDataBasesArr, VCount);
    for I := 0 to VCount - 1 do begin
      VDataBasesArr[I] := PAnsiChar(ADataBases[I]);
    end;

    VResult := router.opt_set_dbpath(VOpt, @VDataBasesArr[0], VCount);
    if VResult <> ROUTER_RESULT_OK then begin
      RiseLibOsmScoutError(nil, 'opt_set_dbpath');
    end;

    // Closest Routable Node Radius in meters (default = 1000)
    VResult := router.opt_set_rnode_radius(VOpt, 1000);
    if VResult <> ROUTER_RESULT_OK then begin
      RiseLibOsmScoutError(nil, 'set_rnode_radius');
    end;

    VResult := router.new(VCtx, VOpt);
    if VResult <> ROUTER_RESULT_OK then begin
      RiseLibOsmScoutError(VCtx, 'new');
    end;

    VResult := router.calc(VCtx, AProfile, @AStartPoint, @ATargetPoint, VCount, VPoints);
    case VResult of

      ROUTER_RESULT_OK: begin
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

      ROUTER_RESULT_NODATA_START: begin
        PrintError(VCtx, 'There is no data in database for start location!');
      end;

      ROUTER_RESULT_NODATA_TARGET: begin
        PrintError(VCtx, 'There is no data in database for target location!');
      end;

      ROUTER_RESULT_NODATA_ROUTE: begin
        PrintError(VCtx, 'The route cannot be built for the given profile. Not enough data in the database!');
      end;

      ROUTER_RESULT_ERROR: begin
        RiseLibOsmScoutError(VCtx, 'calc');
      end;
    else
      raise Exception.CreateFmt(
        '"router_calc" returns unexpected result: %d', [Integer(VResult)]
      );
    end;
  finally
    Math.SetExceptionMask(VExceptionMask);
    router.del(VCtx);
    router.opt_del(VOpt);
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
