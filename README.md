# MRCPlus
Transforms Oculus's MRC system into a spectator camera with customized FOV and smoothing.

This is intended as a spiritual port of the CameraPlus mod.
## Important:
There's a lot to unpack here, so I'll note down a few ideas to write about later:
- Requires a PC with [OBS](https://obsproject.com/) and Oculus's [MRC setup application](https://developer.oculus.com/downloads/package/mixed-reality-capture-tools/).
  - For situations where this is not practical, consider using Henwill8's Replay mod.
- A slow or unstable network connection may impact the VR framerate.
- Quest 2 is recommended for quality and performance.
## Setup:
- Follow Oculus's [MRC setup guide](https://support.oculus.com/2351683478463908). Note that you do not need a real webcam to complete the setup.

- Once the MRCPlus mod is installed, ensure the 'Camera Mode' setting is not set to Disabled.

- In the OBS client, double click on the 'Oculus MRC' source and click "Connect to MRC-enabled game".

If no audio is being captured by OBS, you may need to mirror the 'Oculus MRC' source to another audio device. The mirrored audio can be recorded using an 'audio output capture' source.

## Configuration:
More details coming soon!
## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) - [vscode-bsqm](https://github.com/raftario/vscode-bsqm) and [this template](https://github.com/raftario/bmbf-mod-template)
