unit libosmscout_route;

interface

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}

uses
  SysUtils;

const
  libosmscout_route_dll = 'libosmscout_route.dll';

{$MINENUMSIZE 4}
type
  uint32_t = Cardinal;

  TRouteProfile = (
    ROUTE_PROFILE_CAR,
    ROUTE_PROFILE_BIKE,
    ROUTE_PROFILE_FOOT,
    ROUTE_PROFILE_UNDEF
  );

  TRouteCalcResult  = (
    CALC_RESULT_OK,
    CALC_RESULT_ERROR,
    CALC_RESULT_NODATA_START,
    CALC_RESULT_NODATA_TARGET,
    CALC_RESULT_NODATA_ROUTE
  );

  point_t = packed record
    lon, lat: double;
  end;
  ppoint_t = ^point_t;

  router_version_t = record
    libosmscout_db_file_version: uint32_t;
    libosmscout_commit_hash: PAnsiChar;
  end;
  prouter_version_t = ^router_version_t;

var
  router: packed record
    Handle: THandle;

    init: procedure(); cdecl;

    new: function(out ctx: pointer; const db_path: PAnsiChar): boolean; cdecl;
    new_multi: function(out ctx: pointer; const db_path: PAnsiChar; db_count: uint32_t): boolean; cdecl;
    del: procedure(ctx: pointer); cdecl;

    calc: function(ctx: pointer; profile: TRouteProfile; const p1, p2: ppoint_t;
            out out_count: uint32_t; out out_points: ppoint_t): TRouteCalcResult; cdecl;

    clear: procedure(ctx: pointer); cdecl;
    get_error_message: function(ctx: pointer): PAnsiChar; cdecl;

    get_version: function(): prouter_version_t; cdecl;
  end;

type
  ELibOsmScoutRouteError = class(Exception);

procedure LibOsmScoutRouteInitialize(const dllname: string = libosmscout_route_dll);
function IsLibOsmScoutRouteAvailable(const dllname: string = libosmscout_route_dll): Boolean;

procedure RiseLibOsmScoutError(const ACtx: Pointer; const AFuncName: string);

implementation

uses
  Windows,
  SyncObjs;

var
  GLock: TCriticalSection = nil;
  GInitialized: Boolean = False;

function GetFullLibName(const dllname: string): string; inline;
begin
  Result := ExtractFilePath(ParamStr(0)) + dllname;
end;

function IsLibOsmScoutRouteAvailable(const dllname: string): Boolean;
begin
  try
    if not FileExists(GetFullLibName(dllname)) then begin
      Result := False;
      Exit;
    end;
    if not GInitialized then begin
      LibOsmScoutRouteInitialize(dllname);
    end;
    Result := router.Handle > 0;
  except
    Result := False;
  end;
end;

procedure LibOsmScoutRouteInitialize(const dllname: string);
const
  CFuncNames: array[0..7] of string = (
    'init', 'new', 'new_multi', 'del', 'calc', 'clear', 'get_error_message', 'get_version'
  );
var
  I: Integer;
  P: PPointer;
  VHandle: THandle;
begin
  GLock.Acquire;
  try
    VHandle := 0;
    if not GInitialized then
    try
      GInitialized := True;
      VHandle := SafeLoadLibrary(GetFullLibName(dllname));
      if VHandle = 0 then begin
        VHandle := SafeLoadLibrary(dllname);
      end;
      if VHandle = 0 then begin
        raise ELibOsmScoutRouteError.CreateFmt('Unable to load library %s', [dllname]);
      end;

      P := @@router.init;
      for I := Low(CFuncNames) to High(CFuncNames) do begin
        P^ := GetProcAddress(VHandle, PChar('router_' + CFuncNames[I]));
        if P^ = nil then begin
          raise ELibOsmScoutRouteError.CreateFmt(
            'Unable to find %s() in %s', [CFuncNames[I], dllname]
          );
        end;
        Inc(P);
      end;

      router.Handle := VHandle;
      router.init;

    except
      on E: Exception do begin
        if VHandle <> 0 then begin
          FreeLibrary(VHandle);
        end;
        router.Handle := 0;
        raise;
      end;
    end;
  finally
    GLock.Release;
  end;
end;

procedure RiseLibOsmScoutError(const ACtx: Pointer; const AFuncName: string);
var
  VErr: AnsiString;
  VMsg: string;
begin
  VErr := '';
  if ACtx <> nil then begin
    VErr := router.get_error_message(ACtx);
  end;
  if VErr = '' then begin
    VMsg := Format(
      'Function "router_%s" failed with no error message!', [AFuncName]
    );
  end else begin
    VMsg := Format(
      'Function "router_%s" failed with error: ' + #13#10 + '%s', [AFuncName, VErr]
    );
  end;
  raise ELibOsmScoutRouteError.Create(VMsg);
end;

initialization
  router.Handle := 0;
  GLock := TCriticalSection.Create;

finalization
  FreeAndNil(GLock);
  if router.Handle > 0 then begin
    FreeLibrary(router.Handle);
    router.Handle := 0;
  end;

end.
