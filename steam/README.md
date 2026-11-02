# steam/

SteamPipe upload configuration. Fill IDs after creating the app in Steamworks.

| What | Value |
|---|---|
| App ID | `<appid>` |
| Windows depot | `<appid+1>` |
| Linux depot | `<appid+2>` |
| macOS depot (optional) | `<appid+3>` |

- `app_build.vdf.template`, `depot_windows.vdf.template`, `depot_linux.vdf.template` → copy without `.template` and fill IDs.
- `sdk/` (gitignored): unzip the Steamworks SDK here.
- Upload (human runs it, never an agent with your credentials):
  `steamcmd +login <username> +run_app_build "$(pwd)/steam/app_build_<appid>.vdf" +quit`
- Set live on a **beta branch** first; promote to default after testing on a clean machine / Steam Deck.
