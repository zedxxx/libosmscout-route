unit libosmscout_route_test;

interface

uses
  libosmscout_route;

procedure DoTest(
  const ADatabasePath: AnsiString;
  const AStartPoint: point_t;
  const ATargetPoint: point_t
);

implementation

uses
  Math,
  SysUtils;

procedure RiseError(const ACtx: Pointer; const AFuncName: string);
var
  VErr: PAnsiChar;
  VErrStr: string;
begin
  VErrStr := 'Unknown error';
  if ACtx <> nil then begin
    VErr := router.get_error_message(ACtx);
    if VErr <> '' then begin
      VErrStr := string(AnsiString(VErr));
    end;
  end;
  raise ELibOsmScoutRouteError.Create(
    '"router_' + AFuncName + '" failed with error: ' + VErrStr
  );
end;

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

procedure DoTest(
  const ADatabasePath: AnsiString;
  const AStartPoint: point_t;
  const ATargetPoint: point_t
);
var
  I: Integer;
  VCtx: Pointer;
  VCount: uint32_t;
  VPoints: ppoint_t;
  VCalcResult: TRouteCaclResult;
  {$IF CompilerVersion >= 33}
  VExceptionMask: TArithmeticExceptionMask;
  {$ELSE}
  VExceptionMask: TFPUExceptionMask;
  {$IFEND}
begin
  LibOsmScoutRouteInitialize;

  VExceptionMask := Math.SetExceptionMask(
    [exInvalidOp, exDenormalized, exUnderflow, exPrecision]
  );

  VCtx := nil;
  try
    if not router.new(VCtx, PAnsiChar(ADatabasePath)) then begin
      RiseError(VCtx, 'new');
    end;

    VCalcResult := router.calc(VCtx, ROUTE_PROFILE_BIKE, @AStartPoint, @ATargetPoint, VCount, VPoints);

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
        RiseError(VCtx, 'calc');
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
