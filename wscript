subdirs = '''
  feeds/stocks/nasdaq-rt
  plotting/gnuplot
  tools/replay
  decision-makers/intraday/simple
'''

def options(opt):
        opt.recurse(subdirs)

def configure(conf):
        conf.recurse(subdirs)

def build(bld):
        bld.recurse(subdirs)

from waflib.TaskGen import feature, before_method, after_method
@feature('cxx')
@after_method('process_source')
@before_method('apply_incpaths')
def add_includes_paths(self):
   incs = set(self.to_list(getattr(self, 'includes', '')))
   for x in self.compiled_tasks:
       incs.add(x.inputs[0].parent.path_from(self.path))
   self.includes = list(incs)
