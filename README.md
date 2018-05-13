# Small N Stats Discounting Model Selector (Qt)

Discounting Model Selector is a Qt-based application that easily facilitate many complex calculations in delay discounting research, including the fitting of multiple models of intertemporal choice as well as a Bayesian model comparison method, using a friendly spreadsheet-based approach.

Features include:

  - Discounting Model selection using Bayesian Information Criterion (BIC) compare models (Franck et al., 2015)
  - Identification of best performing model, with corresponding cross-model metrics
  - Provision of Johnson & Bickel (2008) test for non-systematic data (Optional)
  - Easily imports common file formats into the application's spreadsheet (.xlsx)
  - Wide range of model outputs/metrics, saveable in common spreadsheet file formats
  - Cross-platform, supports Windows, Mac, and Linux (only Ubuntu tested)
  - Saves figures in popular image formats
  - (EXPERIMENTAL) Numerical integration as a drop-in alternative to ED50 (i.e., log and normal model-based AUC)

### Version
------
1.8.2 (beta)

### Changelog
------
 * 1.8.2 - Added power model, many bugfixes and UI tweaks
 * 1.8.1 - Added parabolic model
 * 1.8.0 - Added differential evolution methods for derivative-free fitting
 * 1.7.2 - Better charting libaries
 * 1.7.1 - Bug fixes
 * 1.7.0 - Fitting options added, with more customizations for model fitting
 * 1.6.0 - Big plotting enhancements, better memory management with bigger data and more plots
 * 1.5.1 - Expanded plots, included residual plotting and probabilities as potential displays
 * 1.5.0 - Added obnoxious, brute force options to exploring optimal start parameters
 * 1.4.3 - Perf improvements
 * 1.4.0 - Added additional models
 * 1.3.0 - Added numerical integration
 * 1.2.0 - Migrated from R to ALGLIB

### Model Candidates (Models in selection protocol)
------
* **Noise Model**: Intecept-only comparison model (included by default)
* **Exponential Model**: Samuelson, P. A. (1937). A note on measurement of utility. The Review of Economic Studies, 4(2), 155-161. https://doi.org/10.2307/2967612
* **Hyperbolic Model**: Mazur, J. E. (1987). An adjusting procedure for studying delayed reinforcement. In Quantitative analysis of behavior: Vol. 5. The effect of delay and intervening events on reinforcement value (pp. 55-73). Hillsdale, NJ: Erlbaum.
* **Loewenstein & Prelec Model**: Loewenstein, G., and D. Prelec. “Anomalies in Intertemporal Choice: Evidence and an Interpretation.” The Quarterly Journal of Economics 107, no. 2 (May 1, 1992): 573–97. https://doi.org/10.2307/2118482.
* **Beta Delta Model**: Laibson, D. (1997). Golden eggs and hyperbolic discounting. The Quarterly Journal of Economics, 112(2), 443-478. https://doi.org/10.1162/003355397555253
* **Green & Myerson Model**: Green, L., & Myerson, J. (2004). A discounting framework for choice with delayed and probabilistic rewards. Psychological Bulletin, 130(5), 769-792. https://doi.org/10.1037/0033-2909.130.5.769
* **Rachlin Model**: Rachlin, H. (2006). Notes on discounting. Journal of the Experimental Analysis of Behavior, 85(3), 425-435. https://doi.org/10.1901/jeab.2006.85-05
* **Ebert & Prelec Model**: Ebert, J. E. J., & Prelec, D. (2007). The Fragility of Time: Time-Insensitivity and Valuation of the Near and Far Future. Management Science, 53(9), 1423-1438. https://doi.org/10.1287/mnsc.1060.0671
* **Bleichrodt et al. Constant Relative Decreasing Impatience Model**: Bleichrodt, H., Rohde, K. I. M., & Wakker, P. P. (2009). Non-hyperbolic time inconsistency. Games and Economic Behavior, 66(1), 27-38. https://doi.org/10.1016/j.geb.2008.05.007
* **Parabolic Model**: Hartmann, M. N., Hager, O. M., Tobler, P. N., & Kaiser, S. (2013). Parabolic discounting of monetary rewards by physical effort. Behavioural Processes, 100, 192-196. https://doi.org/10.1016/j.beproc.2013.09.014
* **Power Model (2 parameter)**: Białaszek, W., Marcowski, P., & Ostaszewski, P. (2017). Physical and Cognitive Effort Discounting across Different Reward Magnitudes: Tests of Discounting Models. PLOS ONE 12(7): e0182353. https://doi.org/10.1371/journal.pone.0182353

### Usage
------
![Alt text](screencaps/SpreadsheetInterface.gif?raw=true "Spreadsheet Interface")
![Alt text](screencaps/SpreadsheetOptions.gif?raw=true "Range of Options")
![Alt text](screencaps/Analyses.gif?raw=true "Analyses")

### Referenced Works (F/OSS software)
------
The Small N Stats Discounting Model Selector uses a number of open source projects to work properly:
* ALGLIB (3.11.0) - GPLv3 Licensed. Copyright 1994-2017 ALGLIB Project. [Site](http://www.alglib.net)
* Differential Evolution - Copyright 2017 Milos Stojanovic Stojke. [Github](https://github.com/milsto/differential-evolution)
* Qt Framework (5.8) - LGPL 3.0 (GPL 3.0 Licensed). Copyright (C). Qt Team [Site](https://www.qt.io/)
* Qt Examples (Spreadsheet, Recent Files) - BSD-3 Clause. Copyright (C). Qt Team [Site](https://www.qt.io/)
* Tango icon set - Public Domain Licensed. Copyright (C) 2009, "Tango Desktop Project" [Site](http://tango.freedesktop.org/Tango_Desktop_Project)
* QtXlsx - MIT Licensed. Copyright (C) 2013, Debao Zhang <hello@debao.me> [Site](https://github.com/dbzhang800/QtXlsxWriter)
* BDS R Script - GPLv2+ (GPL 3.0 Licensed). Copyright (C) 2016, Dr. Chris Franck, Virginia Tech - Department of Statistics.
* QCustomPlot - GPLv3 Licensed. Copyright (C) 2011-2018. Emanuel Eichhammer [Site](www.qcustomplot.com)

------
### Referenced Works (academic works)
The Small N Stats Discounting Model Selector is based on the following academic works:

* Johnson, M. W., & Bickel, W. K. (2008). An algorithm for identifying nonsystematic delay-discounting data. Experimental and Clinical Psychopharmacology, 16(3), 264-274. https://doi.org/10.1037/1064-1297.16.3.264
* Franck, C. T., Koffarnus, M. N., House, L. L., & Bickel, W. K. (2015). Accurate characterization of delay discounting: a multiple model approach using approximate Bayesian model selection and a unified discounting measure. Journal of the Experimental Analysis of Behavior, 103(1), 218-233. https://doi.org/10.1002/jeab.128
* Gilroy, S. P., Franck, C. T., & Hantula, D. A. (2017). The discounting model selector: Statistical software for delay discounting applications. Journal of the Experimental Analysis of Behavior, 107(3), 388-401. https://doi.org/10.1002/jeab.257
* Gilroy, S. P., & Hantula, D. A. (2018). Discounting model selection with area-based measures: A case for numerical integration: Model Selection with Numerical Integration. Journal of the Experimental Analysis of Behavior. https://doi.org/10.1002/jeab.318

### Acknowledgements and Credits
------
* Donald A. Hantula, Decision Making Laboratory, Temple University [Site](http://astro.temple.edu/~hantula/)
* Chris Franck, Laboratory for Interdisciplinary Statistical Analysis - Virginia Tech

### Installation
------
No other packages are required. Simply build and run or install and run.

### Download
------
All downloadable binaries, if/when posted, will be hosted at [Small N Stats](http://www.smallnstats.com).

### Development
------
Want to contribute? Great! Emails or PM's are welcome.

### Related Projects
------
An R-based package with the methods included in this project has been ported for more advanced users who wish to more flexibly access approximate Bayesian model selection for delay discounting projects. This can be found at [discountingtools](https://www.github.com/miyamot0/discountingtools).

### Todos
------
 - Enhance visual figures and output
 - Add additional metrics and output
 - Explore additional, potential model candidates
 - ~~Automated updates, online repository-based management~~

### License
----
Discounting Model Selector (Qt) - Copyright 2017, Shawn P. Gilroy. GPL-Version 3
