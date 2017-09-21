BeginSubConfig(structConfigTF, configTF, configStandalone, "TF", 't')
AddOption(bunchSim, bool, false, "SIMBUNCHES", 0, "Simulate correct bunch interactions instead of placing only the average number of events")
AddOption(nMerge, int, 0, "MERGE", 0, "Merge n events in a timeframe", min(0))
AddOption(averageDistance, float, 50., "MERGEDIST", 0, "Average distance of events merged into timeframe", min(0.f))
AddOption(randomizeDistance, bool, true, "RAND", 0, "Randomize distance around average distance")
AddOption(shiftFirstEvent, bool, true, "FIRST", 0, "Also shift the first event in z when merging events to a timeframe")
AddOption(bunchCount, int, 12, "BUNCHCOUNT", 0, "Number of bunches per train")
AddOption(bunchSpacing, int, 50, "BUNCHSPACING", 0, "Spacing between benches in ns")
AddOption(bunchTrainCount, int, 48, "BUNCHTRAINCOUNT", 0, "Number of bunch trains")
AddOption(abortGapTime, int, (500 * 25), "ABORTGAP", 0, "Length of abort gap in ns")
AddOption(interactionRate, int, 50000, "RATE", 0, "Instantaneous interaction rate")
AddOption(timeFrameLen, int, (1000000000 / 44), "TF", 0, "Timeframe len in ns")
EndConfig()

BeginSubConfig(structConfigQA, configQA, configStandalone, "QA", 'q')
AddOptionVec(compareInputs, const char*, "INPUT", 0, "Read histogram from these input files and include them in the output plots")
AddOptionVec(compareInputNames, const char*, "INPUTNAME", 0, "Legend entries for data from comparison histogram files")
AddOption(name, const char*, NULL, "NAME", 0, "Legend entry for new data from current processing")
AddOption(output, const char*, NULL, "HISTOUT", 0, "Store histograms in output root file", def("histograms.root"))
EndConfig()

BeginConfig(structConfigStandalone, configStandalone)
AddOption(RUNGPU, bool, true, "GPU", 'g', "Use GPU for processing", message("GPU processing: %s"))
AddOptionSet(RUNGPU, bool, false, "CPU", 'c', "Use CPU for processing", message("CPU enabled"))
AddOption(noprompt, bool, true, "PROMPT", 0, "Do prompt for keypress before exiting", def(false))
AddOption(continueOnError, bool, false, "CONTINUE", 0, "Continue processing after an error")
AddOption(writeoutput, bool, false, "WRITE", 0, "Write tracks found to text output file")
AddOption(writebinary, bool, false, "WRITEBINARY", 0, "Write tracks found to binary output file")
AddOption(DebugLevel, int, 0, "DEBUG", 'd', "Set debug level")
AddOption(seed, int, -1, "SEED", 0, "Set srand seed (-1: random)")
AddOption(cleardebugout, bool, false, "CLEARDEBUG", 0, "Clear debug output file when processing next event")
AddOption(sliceCount, int, -1, "SLICECOUNT", 0, "Number of slices to process (-1: all)", min(-1), max(36))
AddOption(forceSlice, int, -1, "SLICE", 0, "Process only this slice (-1: disable)", min(-1), max(36))
AddOption(cudaDevice, int, -1, "CUDA", 0, "Set GPU device to use (-1: automatic)")
AddOption(NEvents, int, -1, "N", 'n', "Number of events to process (-1; all)", min(1))
AddOption(StartEvent, int, 0, "S", 's', "First event to process", min(0))
AddOption(merger, int, 1, "MERGER", 0, "Run track merging / refit", min(0), max(1))
AddOption(runs, int, 1, "RUNS", 'r', "Number of iterations to perform (repeat each event)", min(0))
AddOption(runs2, int, 1, "RUNS2", 0, "Number of iterations to perform (repeat full processing)", min(0))
AddOption(EventsDir, const char*, "pp", "EVENTS", 'e', "Directory with events to process", message("Reading events from Directory events%s"))
AddOption(OMPTHreads, int, -1, "OMP", 't', "Number of OMP threads to run (-1: all)", min(-1), message("Using %d OMP threads"))
AddOption(eventDisplay, bool, false, "DISPLAY", 'd', "Show standalone event display", message("Event display: %s"))
AddOption(qa, bool, false, "QA", 'q', "Enable tracking QA", message("Running QA: %s"))
AddOption(resetids, bool, false, "ENUMERATECLUSTERIDS", 0, "Enumerate cluster IDs when loading clusters overwriting predefined IDs")
AddOption(lowpt, bool, false, "LOWPT", 'l', "Special treatment of low-Pt tracks")
AddOption(nways, int, 1, "3WAY", 0, "Use 3-way track-fit", set(3), min(1))
AddOption(dzdr, float, 0., "DZDR", 0, "Use dZ/dR search window instead of vertex window", set(2.5f))
AddOption(cont, bool, false, "CONT", 0, "Process continuous timeframe data")
AddOption(outputcontrolmem, unsigned long long int, 0, "OUTPUTMEMORY", 0, "Use predefined output buffer of this size", min(0ull), message("Using %lld bytes as output memory"))
AddOption(affinity, int, -1, "AFFINITY", 0, "Pin CPU affinity to this CPU core", min(-1), message("Setting affinity to restrict on CPU %d"))
AddOption(fifo, bool, false, "FIFO", 0, "Use FIFO realtime scheduler", message("Setting FIFO scheduler: %s"))
AddSubConfig(structConfigTF, configTF)
AddSubConfig(structConfigQA, configQA)
EndConfig()
