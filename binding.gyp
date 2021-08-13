{
  "targets": [
    {
      "target_name": "RuleTextService",
      "sources": [ "./csrc/RuleTextService.cc", "./csrc/RuleTextProcessor.cc" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ]
    }
  ]
}