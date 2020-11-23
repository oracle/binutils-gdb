PARSE_AND_LIST_OPTIONS_CET='
  fprintf (file, _("\
  -z ibtplt                   Generate IBT-enabled PLT entries\n\
  -z ibt                      Generate GNU_PROPERTY_X86_FEATURE_1_IBT\n\
  -z shstk                    Generate GNU_PROPERTY_X86_FEATURE_1_SHSTK\n\
  -z cet-report=[none|warning|error] (default: none)\n"));
'
PARSE_AND_LIST_ARGS_CASE_Z_CET='
      else if (strcmp (optarg, "ibtplt") == 0)
	link_info.ibtplt = TRUE;
      else if (strcmp (optarg, "ibt") == 0)
	link_info.ibt = TRUE;
      else if (strcmp (optarg, "shstk") == 0)
	link_info.shstk = TRUE;
      else if (strncmp (optarg, "cet-report=", 11) == 0)
	{
	  if (strcmp (optarg + 11, "none") == 0)
	    link_info.cet_report = 0;
	  else if (strcmp (optarg + 11, "warning") == 0)
	    link_info.cet_report = 1;
	  else if (strcmp (optarg + 11, "error") == 0)
	    link_info.cet_report = 2;
	  else
	    einfo (_("%F%P: invalid option for -z cet-report=: %s\n"),
		   optarg + 11);
	}
'

PARSE_AND_LIST_OPTIONS="$PARSE_AND_LIST_OPTIONS $PARSE_AND_LIST_OPTIONS_CET"
PARSE_AND_LIST_ARGS_CASE_Z="$PARSE_AND_LIST_ARGS_CASE_Z $PARSE_AND_LIST_ARGS_CASE_Z_CET"
