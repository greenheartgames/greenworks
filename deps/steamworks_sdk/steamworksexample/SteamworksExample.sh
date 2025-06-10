#!/bin/bash
#
# This is a script which runs the SteamworksExample in the Steam runtime

# The program location
TOP=$(cd "${0%/*}" && echo ${PWD})

PROGRAM="${TOP}/SteamworksExampleLinux"

log () {
    ( echo "[$$]: $*" >&2 ) || :
}

# Require LDLP scout runtime environment
if [ -n "${STEAM_RUNTIME-}" ]; then
	log "Detected scout LDLP runtime."
	# continue
else
	log "Relaunch under scout LDLP runtime."
	log exec "$HOME/.steam/bin/steam-runtime/run.sh" "$0" "$@"
	exec "$HOME/.steam/bin/steam-runtime/run.sh" "$0" "$@"
	# unreachable
fi

# The public SDK binary links with -Wl,--rpath=$ORIGIN and doesn't need this,
# But the binary produced in-tree at Valve does
export LD_LIBRARY_PATH=${TOP}:${LD_LIBRARY_PATH-}

cd "${TOP}"

exec "${PROGRAM}" "$@"

# vi: ts=4 sw=4 expandtab
