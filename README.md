# StimUSTO

Stimulus presentation platefrom for real time EEG based Brain-Computer Interface experiments, developed at the LARESI Lab, USTO-MB. Algeria (work in progress)

- Works on top of [OpenVibe](http://openvibe.inria.fr) platefrom, which guarantees real-time EEG signal processing.
- Developed using [Qt](https://www.qt.io) framework with C++.

## Stimulations
The main goal of this project is to facilitate novel High-Speed Stimulus presentation development for Brain-Computer Interface research and applications, by using an Open Source Cross-Platform native GUI framework.

| ERP           | SSVEP         | Hybrid  |
| ------------- |:-------------:| -----:|
|  Signel character spellers    | Square ON/OFF |ERP & SSVEP |
| [Face / Inverted Face speller](https://iopscience.iop.org/article/10.1088/1741-2560/9/2/026018)      | [Sampled Sinusoidal Stimulation](https://www.tandfonline.com/doi/abs/10.1080/2326263X.2014.944469) (with OpenGL)     |    |
| Colored Face/Inverted Face    |               |         |
---

## Installation

### Using Qt creator

Before building the project, make sure Qt (version 5.x) and Qt creator are installed. then load the project and open it in Qt create. After that build it as any other Qt project.

---

## Usage

Before any use, open OpenVibe Acquisition Server (for reading EEG signal in real-time) and OpenVibe Designer (for real-time signal processing)

### with GUI
The GUI will pop up after succeful build, after that you can re-open it by double click on the ```StimUSTO.exe``` in the folder ```build\release\release``` folder.
The configuration panel is what is shown first, there the Stimulation paradigm and configurations are edited and started.

- Connect to OpenVibe acquisiton server by clicking on connect (with default configurations of host and port). This connection is used for passing event markers to the OpenVibe Acquisition Server via a TCP connection.
- Modify the parameters as suitable to your experiment needs.
- Click on Init[Speller|SSVEP|Hybrid] to init one of the paradigm.
- Click on Start at the control panel to start experiment.

### No GUI - CLI
This functionality requires specifying a config file in a JSON format for experiment configurations with the ```--no-gui``` and ```--file``` options (a config example is found in the /config folder). Here the connection to OpenVibe acquisiton server is done automatically.

in command prompt type the following command:
```
StimUSTO.exe --nogui True --file "/config/default_.json"
```

---

## Citation
```
@misc{bekhelifi2022fast,
    title={Towards Fast Single-Trial Online ERP based Brain-Computer Interface using dry EEG electrodes and neural networks: a pilot study},
    author={Okba Bekhelifi and Nasr-Eddine Berrached},
    year={2022},
    eprint={2211.10352},
    archivePrefix={arXiv},
    primaryClass={eess.SP}
}
```
