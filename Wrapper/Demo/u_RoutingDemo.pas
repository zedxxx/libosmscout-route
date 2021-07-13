unit u_RoutingDemo;

interface

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}

uses
  libosmscout_route;

procedure PrintRoute(
  const AStartPoint: point_t;
  const ATargetPoint: point_t;
  const ADatabases: array of AnsiString
);

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

{$IFNDEF FPC}
  {$IF CompilerVersion >= 33}
    {$DEFINE USE_NEW_EXCEPTION_MASK}
  {$IFEND}
{$ENDIF}

procedure PrintRoute(
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
  VDataBasesArr: array of PAnsiChar {$IFDEF FPC} = nil {$ENDIF};
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

    VCalcResult := router.calc(VCtx, ROUTE_PROFILE_BIKE, @AStartPoint,
      @ATargetPoint, VCount, VPoints);

    case VCalcResult of

      CALC_RESULT_OK: begin
        for I := 0 to VCount - 1 do begin
          PrintPoint(VPoints);
          Inc(VPoints);
        end;

        router.clear(VCtx);
      end;

      CALC_RESULT_NODATA: begin
        Writeln('There is no data in database for this location');
      end;

      CALC_RESULT_ERROR: begin
        RiseLibOsmScoutError(VCtx, 'calc');
      end;
    else
      raise Exception.CreateFmt(
        'Unexpected result value: %d', [Integer(VCalcResult)]
      );
    end;
  finally
    Math.SetExceptionMask(VExceptionMask);
    router.del(VCtx);
  end;
end;

end.
