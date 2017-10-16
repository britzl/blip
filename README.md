# Blip
Blip is a library to procedurally generate and play sound effects for games. The library is based on the [Sfxr](https://github.com/grimfang4/sfxr) project.

# Installation
You can use Blip in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

https://github.com/britzl/blip/archive/master.zip

Or point to the ZIP file of a [specific release](https://github.com/britzl/blip/releases).

# Usage
Blip is very easy to use. You need to start by creating one or more Blip sounds. Blip sounds can be created by calling one of the predefined sound effect groups and pass a seed. The seed is a number that is used to generate a specific version of the sound effect. Passing in a certain seed will always result in the same sound. Use the [example app](example/) to find seeds that you like and use them in your own project.

	local seed = 1234
	local sound = blip.blip(seed)
	local sound = blip.jump(seed)
	local sound = blip.hurt(seed)
	local sound = blip.powerup(seed)
	local sound = blip.explosion(seed)
	local sound = blip.laser(seed)
	local sound = blip.pickup(seed)

 Once you have created a blip sound you can play it by calling the play function:

 	blip.play(sound)

# Try it!
Try the [HTML5 version of the example app](https://github.com/Lerg).

# Credits
* [Sfxr](https://github.com/grimfang4/sfxr) by [Dr.Petter](http://www.drpetter.se/)
* OpenAL wrapper code based on [extension-openal](https://github.com/Lerg/extension-openal) by [Sergey Lerg](https://github.com/Lerg)
