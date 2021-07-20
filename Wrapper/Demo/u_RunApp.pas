unit u_RunApp;

interface

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}

procedure RunApp;

implementation

uses
  SysUtils,
  libosmscout_route,
  u_RoutingDemo;

function ReadCoordinate(const AStr: string): Double;
var
  VFormatSettings: TFormatSettings;
begin
  VFormatSettings.DecimalSeparator := '.';
  Result := StrToFloat(AStr, VFormatSettings);
end;

procedure RunApp;
var
  I: Integer;
  VDatabases: array of AnsiString {$IFDEF FPC} = nil {$ENDIF};
  VStartPoint, VTargetPoint: point_t;
begin
  try
    if ParamCount < 5 then begin
      Writeln('Usage:');
      Writeln('  ',  ExtractFileName(ParamStr(0)), ' START TARGET DATABASE 1 [... DATABASE X]');
      Writeln;
      Writeln('START                       Start coordinate');
      Writeln('TARGET                      Target coordinate');
      Writeln('DATABASE 1 [... DATABASE X] Directories with databases to use');
      Exit;
    end;

    VStartPoint.lat := ReadCoordinate(ParamStr(1));
    VStartPoint.lon := ReadCoordinate(ParamStr(2));

    VTargetPoint.lat := ReadCoordinate(ParamStr(3));
    VTargetPoint.lon := ReadCoordinate(ParamStr(4));

    SetLength(VDatabases, ParamCount - 4);
    for I := 0 to Length(VDatabases) - 1 do begin
      VDatabases[I] := AnsiString(ParamStr(I + 1 + 4));
    end;

    PrintRoute(VStartPoint, VTargetPoint, VDatabases);
  except
    on E: Exception do begin
      Writeln(E.ClassName, ': ', E.Message);
    end;
  end;
end;

end.
