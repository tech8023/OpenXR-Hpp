#!/usr/bin/python3
#
# Copyright (c) 2013-2017 The Khronos Group Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import os
import re
import sys
import time

OPENXR = os.getenv("OPENXR_REPO")
if not OPENXR:
    OPENXR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'OpenXR-SDK-Source'))
sys.path.append(os.path.join(OPENXR, 'src', 'scripts'))
sys.path.append(os.path.join(OPENXR, 'specification', 'scripts'))

from automatic_source_generator import AutomaticSourceGeneratorOptions
from cpp_generator import CppGenerator
from generator import write
from reg import Registry
from xrconventions import OpenXRConventions

# Simple timer functions
startTime = None


def startTimer(timeit):
    global startTime
    startTime = time.process_time()


def endTimer(timeit, msg):
    global startTime
    endTime = time.process_time()
    if timeit:
        write(msg, endTime - startTime, file=sys.stderr)
        startTime = None


def makeREstring(strings, default=None):
    """Turn a list of strings into a regexp string matching exactly those strings."""
    if strings or default is None:
        return '^(' + '|'.join((re.escape(s) for s in strings)) + ')$'
    return default


def genTarget(args):
    """
    Create an API generator and corresponding generator options based on
    the requested target and command line options.

    This is encapsulated in a function so it can be profiled and/or timed.
    The args parameter is an parsed argument object containing the following
    fields that are used:
    - target - target to generate
    - directory - directory to generate it in
    - protect - True if re-inclusion wrappers should be created
    - extensions - list of additional extensions to include in generated interfaces"""

    # Create generator options with specified parameters
    header = args.target
    if 'dispatch' in args.target:
        # Don't omit anything when generating dispatchers.
        removeExtensions = None
    else:
        removeExtensions = makeREstring((
            # Atom not projecting right?
            "XR_MSFT_controller_model",
            # Projection of static string fails
            "XR_MSFT_spatial_graph_bridge",
        ))

    # Turn lists of names/patterns into matching regular expressions
    emitExtensionsPat = makeREstring(args.emitExtensions, '.*')
    featuresPat = makeREstring(args.feature, '.*')
    options = AutomaticSourceGeneratorOptions(
        conventions=OpenXRConventions(),
        filename=header,
        directory=args.directory,
        apiname='openxr',
        profile=None,
        versions=featuresPat,
        emitversions=featuresPat,
        defaultExtensions='openxr',
        addExtensions=None,
        removeExtensions=removeExtensions,
        emitExtensions=emitExtensionsPat)

    if not args.quiet:
        write('* Building', options.filename, file=sys.stderr)
        write('* options.versions          =', options.versions, file=sys.stderr)
        write('* options.emitversions      =', options.emitversions, file=sys.stderr)
        write('* options.defaultExtensions =', options.defaultExtensions, file=sys.stderr)
        write('* options.addExtensions     =', options.addExtensions, file=sys.stderr)
        write('* options.removeExtensions  =', options.removeExtensions, file=sys.stderr)
        write('* options.emitExtensions    =', options.emitExtensions, file=sys.stderr)

    gen = CppGenerator(errFile=errWarn,
                       warnFile=errWarn,
                       diagFile=diag,
                       quiet=args.quiet)
    return (gen, options)


# -feature name
# -extension name
# For both, "name" may be a single name, or a space-separated list
# of names, or a regular expression.
if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-defaultExtensions', action='store',
                        default='openxr',
                        help='Specify a single class of extensions to add to targets')
    parser.add_argument('-extension', action='append',
                        default=[],
                        help='Specify an extension or extensions to add to targets')
    parser.add_argument('-removeExtensions', action='append',
                        default=[],
                        help='Specify an extension or extensions to remove from targets')
    parser.add_argument('-emitExtensions', action='append',
                        default=[],
                        help='Specify an extension or extensions to emit in targets')
    parser.add_argument('-feature', action='append',
                        default=[],
                        help='Specify a core API feature name or names to add to targets')
    parser.add_argument('-debug', action='store_true',
                        help='Enable debugging')
    parser.add_argument('-dump', action='store_true',
                        help='Enable dump to stderr')
    parser.add_argument('-diagfile', action='store',
                        default=None,
                        help='Write diagnostics to specified file')
    parser.add_argument('-errfile', action='store',
                        default=None,
                        help='Write errors and warnings to specified file instead of stderr')
    parser.add_argument('-noprotect', dest='protect', action='store_false',
                        help='Disable inclusion protection in output headers')
    parser.add_argument('-profile', action='store_true',
                        help='Enable profiling')
    parser.add_argument('-registry', action='store',
                        default='xr.xml',
                        help='Use specified registry file instead of xr.xml')
    parser.add_argument('-time', action='store_true',
                        help='Enable timing')
    parser.add_argument('-validate', action='store_true',
                        help='Enable group validation')
    parser.add_argument('-o', action='store', dest='directory',
                        default='.',
                        help='Create target and related files in specified directory')
    parser.add_argument('target', metavar='target', nargs='?',
                        help='Specify target')
    parser.add_argument('-quiet', action='store_true', default=False,
                        help='Suppress script output during normal execution.')
    parser.add_argument('-verbose', action='store_false', dest='quiet', default=True,
                        help='Enable script output during normal execution.')

    args = parser.parse_args()

    # This splits arguments which are space-separated lists
    args.feature = [name for arg in args.feature for name in arg.split()]
    args.extension = [name for arg in args.extension for name in arg.split()]

    # create error/warning & diagnostic files
    if args.errfile:
        errWarn = open(args.errfile, 'w', encoding='utf-8')
    else:
        errWarn = sys.stderr

    if args.diagfile:
        diag = open(args.diagfile, 'w', encoding='utf-8')
    else:
        diag = None

    # Create the API generator & generator options
    (gen, options) = genTarget(args)

    # Create the registry object with the specified generator and generator
    # options. The options are set before XML loading as they may affect it.
    reg = Registry(gen, options)

    # Parse the specified registry XML into an ElementTree object
    startTimer(args.time)
    reg.loadFile(args.registry)
    endTimer(args.time, '* Time to make and parse ElementTree =')

    if args.validate:
        reg.validateGroups()

    if args.dump:
        write('* Dumping registry to regdump.txt', file=sys.stderr)
        reg.dumpReg(filehandle=open('regdump.txt', 'w', encoding='utf-8'))

    if args.debug:
        import pdb
        pdb.run('reg.apiGen()')
    elif args.profile:
        import cProfile
        import pstats
        cProfile.run('reg.apiGen()', 'profile.txt')
        p = pstats.Stats('profile.txt')
        p.strip_dirs().sort_stats('time').print_stats(50)
    else:
        startTimer(args.time)
        reg.apiGen()
        if not args.quiet:
            write('* Generated', options.filename, file=sys.stderr)
        endTimer(args.time, '* Time to generate ' + options.filename + ' =')
