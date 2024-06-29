[![img-from-clip](https://snapcraft.io/img-from-clip/badge.svg)](https://snapcraft.io/img-from-clip)

Simple tool which can create images from clipboard contents
and save it as file to specific directory automatically.

Images stored in the clipboard are converted to the target format.
If user copies raw SVG text (e.g. from various UI design tools,
which support "copy as SVG", the image will be stored as-is
if SVG conversion is used).

HTML rendering is also supported. If user copies HTML into clipboard,
the application will render the image into a document of specified
width. Rendering is done using QTextDocument engine, so results may not
be perfect.


### Languages: ENG, KOR, POL


![Screenshot](shared/linux/screenshot.png)

### Lua script

ImgFromClip can make use of Lua scripting language in order to process
the images during capture.

Image metadata is exposed as a following object:
* path - file name of target image
* save_mode - name of the save type (PNG, SVG or JPG)
* width - width of image in pixels
* height - height of image in pixels
* contents - if SVG mode is used, this is raw SVG text
* image - image object

Script must expose single global function:

```lua
    function process_capture(image)
    end
```

### Attributions

App icon:

<a href="https://www.flaticon.com/free-icons/screenshot" title="screenshot icons">Screenshot icons created by icon_small - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/compiler" title="compiler icons">Compiler icons created by Karyative - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/error" title="error icons">Error icons created by Freepik - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/success" title="success icons">Success icons created by Picons - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/clean" title="clean icons">Clean icons created by Freepik - Flaticon</a>
