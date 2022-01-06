program RoutingDemo;

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}

{$APPTYPE CONSOLE}

uses
  SysUtils,
  u_RoutingDemo in 'u_RoutingDemo.pas',
  libosmscout_route in '..\libosmscout_route.pas';

procedure PrintUsage;
begin
  Writeln('Usage:');
  Writeln('  ',  ExtractFileName(ParamStr(0)), ' START TARGET DATABASE 1 [... DATABASE X] [--PROFILE]');
  Writeln;
  Writeln('START                       Start coordinate');
  Writeln('TARGET                      Target coordinate');
  Writeln('DATABASE 1 [... DATABASE X] Directories with databases to use');
  Writeln('[--PROFILE]                 Routing profile:');
  Writeln('                              CAR (default), BIKE, FOOT');
end;

function ReadProfile(const AStr: string): TRouteProfile;
begin
  if (AStr = 'CAR') or (AStr = '') then
    Result := ROUTE_PROFILE_CAR
  else if AStr = 'BIKE' then
    Result := ROUTE_PROFILE_BIKE
  else if AStr = 'FOOT' then
    Result := ROUTE_PROFILE_FOOT
  else
    Result := ROUTE_PROFILE_UNDEF;
end;

function ReadCoordinate(const AStr: string): Double;
var
  VFormatSettings: TFormatSettings;
begin
  VFormatSettings.DecimalSeparator := '.';
  Result := StrToFloat(AStr, VFormatSettings);
end;

procedure DoMain;
var
  I, J: Integer;
  VParam: string;
  VProfile: TRouteProfile;
  VProfileStr: string;
  VDatabases: array of AnsiString;
  VStartPoint, VTargetPoint: point_t;
begin
  VStartPoint.lat := ReadCoordinate(ParamStr(1));
  VStartPoint.lon := ReadCoordinate(ParamStr(2));

  VTargetPoint.lat := ReadCoordinate(ParamStr(3));
  VTargetPoint.lon := ReadCoordinate(ParamStr(4));

  J := 0;
  VProfileStr := '';
  SetLength(VDatabases, ParamCount - 4);

  for I := 0 to Length(VDatabases) - 1 do begin
    VParam := ParamStr(I + 1 + 4);
    if Pos('--', VParam) = 1 then begin
      VProfileStr := UpperCase(Copy(VParam, 3));
      Break;
    end;
    VDatabases[J] := AnsiString(VParam);
    Inc(J);
  end;
  SetLength(VDatabases, J);

  VProfile := ReadProfile(VProfileStr);
  if VProfile = ROUTE_PROFILE_UNDEF then begin
    Writeln('Invalid PROFILE value: ', VProfileStr);
    Exit;
  end;

  PrintRoute(VProfile, VStartPoint, VTargetPoint, VDatabases);
end;

begin
  if ParamCount < 5 then begin
    PrintUsage;
    Exit;
  end;

  try
    PrintLibOsmScoutVersion;
    DoMain;
  except
    on E: Exception do begin
      Writeln(E.ClassName, ': ', E.Message);
    end;
  end;
end.
