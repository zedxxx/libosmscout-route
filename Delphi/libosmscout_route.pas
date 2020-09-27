unit libosmscout_route;

interface

uses
  SysUtils;

const
  libosmscout_route_dll = 'libosmscout_route.dll';

{$MINENUMSIZE 4}
type
  uint32_t = Cardinal;

  route_profile = (
    ROUTE_PROFILE_CAR,
    ROUTE_PROFILE_BIKE,
    ROUTE_PROFILE_FOOT
  );

  point_t = packed record
    lon, lat: double;
  end;
  ppoint_t = ^point_t;

var
  router: packed record
    Handle: THandle;

    new: function(out ctx: pointer; const db_path: PAnsiChar): boolean; cdecl;
    del: function(ctx: pointer): boolean; cdecl;

    calc: function(ctx: pointer; profile: route_profile; const p1, p2: ppoint_t;
            out out_count: uint32_t; out out_points: ppoint_t): boolean; cdecl;

    clear: function(ctx: pointer): boolean; cdecl;
    get_error_message: function(ctx: pointer): PAnsiChar; cdecl;
  end;

type
  ELibOsmScoutRouteError = Exception;

procedure LibOsmScoutRouteInitialize(const dllname: string = libosmscout_route_dll);
function IsLibOsmScoutRouteAvailable: Boolean;

implementation

uses
  Windows,
  SyncObjs;

var
  GLock: TCriticalSection = nil;

function IsLibOsmScoutRouteAvailable: Boolean;
begin
  try
    if router.Handle = 0 then begin
      LibOsmScoutRouteInitialize;
    end;
    Result := router.Handle > 0;
  except
    Result := False;
  end;
end;

procedure LibOsmScoutRouteInitialize(const dllname: string);
const
  CFuncNames: array[0..4] of string = (
    'new', 'del', 'calc', 'clear', 'get_error_message'
  );
var
  I: Integer;
  P: PPointer;
  VHandle: THandle;
begin
  GLock.Acquire;
  try
    VHandle := 0;
    if router.Handle = 0 then
    try
      VHandle := SafeLoadLibrary(ExtractFilePath(ParamStr(0)) + dllname);
      if VHandle = 0 then begin
        VHandle := SafeLoadLibrary(dllname);
      end;
      if VHandle = 0 then begin
        raise ELibOsmScoutRouteError.CreateFmt('Unable to load library %s', [dllname]);
      end;

      P := @@router.new;
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
