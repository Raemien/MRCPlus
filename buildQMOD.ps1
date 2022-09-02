# Builds a .zip file for loading with BMBF
& ./build.ps1
Compress-Archive -Path  "./extern/libs/libbeatsaber-hook_3_8_1.so",`
                        "./build/libmrcplus.so",`
                        "./cover.png",`
                        "./mod.json" -DestinationPath "./MRCPlus.zip" -Update
& copy-item -Force "./MRCPlus.zip" "./MRCPlus.qmod"
