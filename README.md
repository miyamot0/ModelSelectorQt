# Small N Stats Discounting Model Selector (Qt)
Discounting Model Selector is a Qt-based application that easily facilitate many complex calculations, including the fitting of multiple models of intertemporal choice as well as a bayesian model comparison method, using a friendly spreadsheet-based approach.

Features include:
  - Non-linear model fittings (Exponential, Hyperbolic, Quasi-Hyperbolic/b-d, and both Hyperboloid Variants)
  - Discounting Model comparison/competition using Discounting information criterion (BIC) to inform the best/better fitting models (see Franck et al., 2015)
  - ED50 calculations for all models, all parameter fittings, and overall model competition (e.g., best, 2nd best, ...)
  - Identification of best performing model, with corresponding ED50, for similar cross-model discounting indices
  - R-based graphical output in appropriate log space(s) for each model selected
  - Easily imports common file formats into the application's spreadsheet (.xlsx)
  - Wide range of model outputs/metrics, saveable in common spreadsheet file formats
  - Cross-platform support, compile-able for Windows, Mac, and Linux (only Ubuntu tested)

### Version
1.0.0.0 (alpha)

### Changelog
 * 1.0.0.0 - Initial push

### Referenced Works (F/OSS software)
The Small N Stats Discounting Model Selector uses a number of open source projects to work properly:
* R Statistical Package - GPLv2+ Licensed. Copyright (C) 2000-16. The R Core Team [Site](https://www.r-project.org/)
* Qt Framework (5.8) - LGPL 3.0 (GPL 3.0 Licensed). Copyright (c). Qt Team [Site](https://www.qt.io/)
* Qt Examples (Spreadsheet, Recent Files) - BSD-3 Clause. Copyright (c). Qt Team [Site](https://www.qt.io/)

### Referenced Works (R packages/scripts)
The Discounting Model Selector accesses the following R packages to perform statistical methods:
* nls R Package - GPLv2+ Licensed. Copyright (C) 1999-1999 Saikat DebRoy, Douglas M. Bates, Jose C. Pinheiro.
* nls R Package - GPLv2+ Licensed. Copyright (C) 2000-7. The R Core Team.
* base64enc R Package - GPLv2+ Licensed. Copyright (c) 2015, Simon Urbanek
* reshape R Package - MIT Licensed. Copyright (c) 2014, Hadley Wickham.
* BDS R Script - GPLv2+ Licensed. Copyright (c) 2016, Dr. Chris Franck, Virginia Tech - Department of Statistics.

### Referenced Works (academic works)
The Small N Stats Discounting Model Selector is based on the following academic works:
* Franck, C. T., Koffarnus, M. N., House, L. L. & Bickel W. K. (2015). Accurate characterization of delay discounting: a multiple model approach using approximate Bayesian model selection and a unified discounting measure. Journal of the Experimental Analysis of Behavior, 103, 218-33.

### Acknowledgements and Credits
* Donald A. Hantula, Decision Making Laboratory, Temple University [Site](http://astro.temple.edu/~hantula/)
* Chris Franck, Laboratory for Interdisciplinary Statistical Analysis - Virginia Tech

### Installation
You will need the R open-source statistical package for model fitting/charting to be performed.
Once Discounting Model Selector is installed, it will perform a one-time install the necessary R packages (internet required).
Discounting Model Selector is a C++ application, installer binaries can be downloaded or the source can be compiled at your leisure.

### Download
All downloads, if/when posted, will be hosted at [Small N Stats](http://www.smallnstats.com).

### Development
Want to contribute? Great! Emails or PM's are welcome.

### Todos
 - Cross-platform testing, cleaning up OSX-based issues presently

### License
----
Discounting Model Selector (Qt) - Copyright 2017, Shawn P. Gilroy. GPL-Version 3

