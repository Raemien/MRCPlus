& ./build.ps1

& adb push build/libmrcplus.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libmrcplus.so
& adb shell am force-stop com.beatgames.beatsaber
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
