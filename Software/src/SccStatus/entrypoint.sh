#!/bin/sh
set -e

# Start pcscd in background, without polkit
pcscd --foreground --auto-exit --disable-polkit &
sleep 1

# Start your .NET app
dotnet SmartCardStatusApi2.dll
