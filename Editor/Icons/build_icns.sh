mkdir Helium.iconset
sips -z 16 16     Helium/HeliumLogo1024.png --out Helium.iconset/icon_16x16.png
sips -z 32 32     Helium/HeliumLogo1024.png --out Helium.iconset/icon_16x16@2x.png
sips -z 32 32     Helium/HeliumLogo1024.png --out Helium.iconset/icon_32x32.png
sips -z 64 64     Helium/HeliumLogo1024.png --out Helium.iconset/icon_32x32@2x.png
sips -z 128 128   Helium/HeliumLogo1024.png --out Helium.iconset/icon_128x128.png
sips -z 256 256   Helium/HeliumLogo1024.png --out Helium.iconset/icon_128x128@2x.png
sips -z 256 256   Helium/HeliumLogo1024.png --out Helium.iconset/icon_256x256.png
sips -z 512 512   Helium/HeliumLogo1024.png --out Helium.iconset/icon_256x256@2x.png
sips -z 512 512   Helium/HeliumLogo1024.png --out Helium.iconset/icon_512x512.png
cp Helium/HeliumLogo1024.png Helium.iconset/icon_512x512@2x.png
iconutil -c icns Helium.iconset
rm -R Helium.iconset