program routing_test;

{$APPTYPE CONSOLE}

uses
  SysUtils,
  libosmscout_route in 'libosmscout_route.pas',
  libosmscout_route_test in 'libosmscout_route_test.pas';

function ReadCoordinate(const AStr: string): Double;
var
  VFormatSettings: TFormatSettings;
begin
  VFormatSettings.DecimalSeparator := '.';
  Result := StrToFloat(AStr, VFormatSettings);
end;

var
  VDatabasePath: AnsiString;
  VStartPoint, VTargetPoint: point_t;
begin
  try
    if ParamCount < 5 then begin
      Writeln('Usage:');
      Writeln('  ',  ExtractFileName(ParamStr(0)), ' DATABASE START TARGET');
      Writeln;
      Writeln('DATABASE          Directory of the database to use');
      Writeln('START             start coordinate');
      Writeln('TARGET            target coordinate');
      Exit;
    end;

    VDatabasePath := AnsiString(ParamStr(1));

    VStartPoint.lat := ReadCoordinate(ParamStr(2));
    VStartPoint.lon := ReadCoordinate(ParamStr(3));

    VTargetPoint.lat := ReadCoordinate(ParamStr(4));
    VTargetPoint.lon := ReadCoordinate(ParamStr(5));

    DoTest(VDatabasePath, VStartPoint, VTargetPoint);
  except
    on E: Exception do begin
      Writeln(E.ClassName, ': ', E.Message);
    end;
  end;

  Writeln('Press Enter to exit...');
  Readln;
end.
