call "C:\Program Files\Microsoft SDKs\Windows\v7.1\bin\SetEnv.Cmd" /Release /x86

npm install -g nw-gyp

nw-gyp configure --target=0.12.2 --arch=ia32

nw-gyp build

