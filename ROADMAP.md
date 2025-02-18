# Roadmap

## Version 1

- [x] UI backend
   - [X] Windowing backend (pugl)
   - [X] Drawing backend (cairo)
   - [X] Widgets
     - [X] Buttons
     - [X] File dialog
     - [X] Knobs
     - [X] Text
   - [x] UI optimizations
     - [X] Only redraw affected regions
- [ ] DSP backends
   - [ ] Convolver
- [ ] Plugin integration
   - [ ] LV2
   - [ ] VST
- [ ] Plugins
  - [ ] IR convolver


## Future

- UI backend
  - OpenGL backend
  - UI optimizations
    - Avoid redrawing surfaces hidden by other surfaces on the top
- DSP backend
  - FFT
- Plugin integration
  - AudioUnit
- Plugins
  - Equalizer
