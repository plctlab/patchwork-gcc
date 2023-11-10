#!/usr/bin/env python3

# Copyright (C) 2023 Free Software Foundation, Inc.
#
# Script to regenerate FOO.opt.urls files for each FOO.opt in the
# source tree.
#
# This file is part of GCC.
#
# GCC is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3, or (at your option) any later
# version.
#
# GCC is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with GCC; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.  */

DESCRIPTION = """
Parses the generated HTML (from "make html") to locate anchors
for options, then parses the .opt files within the source tree,
and generates a .opt.urls in the source tree for each .opt file,
giving URLs for each option, where it can.

Usage (from build/gcc subdirectory):
  ../../src/gcc/regenerate-opt-urls.py HTML/gcc-14.0.0/ ../../src

To run unit tests:
  ../../src/gcc/regenerate-opt-urls.py HTML/gcc-14.0.0/ ../../src --unit-test
"""

import argparse
import json
import os
from pathlib import Path
from pprint import pprint
import sys
import re
import unittest

def canonicalize_option_name(option_name):
    if option_name.endswith('='):
        option_name = option_name[0:-1]
    return option_name


def canonicalize_url_suffix(url_suffix):
    """
    Various options have anchors for both the positive and
    negative form.  For example -Wcpp has both:
      'gcc/Warning-Options.html#index-Wno-cpp'
      'gcc/Warning-Options.html#index-Wcpp'

    Return a canonicalized version of the url_suffix that
    strips out any "no-" prefixes, for use in deduplication.
    Note that the resulting url suffix might not correspond to
    an actual anchor in the HTML.
    """
    url_suffix = re.sub('index-Wno-', 'index-W', url_suffix)
    url_suffix = re.sub('index-fno-', 'index-f', url_suffix)
    url_suffix = re.sub('_003d$', '', url_suffix)
    url_suffix = re.sub('-([0-9]+)$', '', url_suffix)
    return url_suffix


class Index:
    def __init__(self):
        # Map from option name to set of URL suffixes
        self.entries = {}

    def add_entry(self, matched_text, url_suffix, verbose=False):
        if 'Attributes.html' in url_suffix:
            return
        matched_text = canonicalize_option_name(matched_text)
        if matched_text in self.entries:
            # Partition by canonicalized url_suffixes; add the
            # first url_suffix in each such partition.
            c_new = canonicalize_url_suffix(url_suffix)
            for entry in self.entries[matched_text]:
                c_entry = canonicalize_url_suffix(entry)
                if c_new == c_entry:
                    return
            self.entries[matched_text].add(url_suffix)
        else:
            self.entries[matched_text] = set([url_suffix])

    def get_url_suffixes(self, text):
        text = canonicalize_option_name(text)
        return self.entries.get(text)

    def parse_option_index(self, input_filename, verbose=False):
        with open(input_filename) as f:
            for line in f:
                self.parse_html_line_option_index(line, verbose)

    def parse_html_line_option_index(self, line, verbose=False):
        if verbose:
            print(repr(line))

        # Update for this in the GCC website's bin/preprocess process_html_file:
        #   | sed -e 's/_002d/-/g' -e 's/_002a/*/g' \
        line = line.replace('_002d', '-')
        line = line.replace('_002a', '*')

        # e.g. <a href="Optimize-Options.html#index-fmodulo_002dsched"><code>fmodulo-sched</code></a>
        m = re.search(r'<a href="([\S]+)"><code>([\S]+)</code></a>', line)
        if not m:
            return
        if verbose:
            print(m.groups())
        url_suffix, index_text = m.groups()
        #print(f'{url_suffix=} {index_text=}')
        option = '-' + index_text

        # Strip off "no-" prefixes from options
        if option[:5] == '-Wno-':
            option = '-W' + option[5:]
        if option[:5] == '-fno-':
            option = '-f' + option[5:]

        url_suffix = 'gcc/' + url_suffix
        self.add_entry(option, url_suffix, verbose)


class TestParsingIndex(unittest.TestCase):
    def test_parse_line(self):
        index = Index()
        index.parse_html_line_option_index('<a href="Optimize-Options.html#index-fmodulo_002dsched"><code>fmodulo-sched</code></a>')
        self.assertEqual(index.get_url_suffixes('-fmodulo-sched'),
                         {'gcc/Optimize-Options.html#index-fmodulo-sched'})

    def test_negated_flag(self):
        index = Index()
        index.parse_html_line_option_index('<tr><td></td><td valign="top"><a href="Static-Analyzer-Options.html#index-fno_002danalyzer"><code>fno-analyzer</code></a>:</td><td>&nbsp;</td><td valign="top"><a href="Static-Analyzer-Options.html">Static Analyzer Options</a></td></tr>\n')
        self.assertEqual(index.get_url_suffixes('-fno-analyzer'), None)
        self.assertEqual(index.get_url_suffixes('-fanalyzer'),
                         {'gcc/Static-Analyzer-Options.html#index-fno-analyzer'})

    def test_negated_warning(self):
        index = Index()
        index.parse_html_line_option_index('<tr><td></td><td valign="top"><a href="Warning-Options.html#index-Wno_002dalloca"><code>Wno-alloca</code></a>:</td><td>&nbsp;</td><td valign="top"><a href="Warning-Options.html">Warning Options</a></td></tr>\n')
        self.assertEqual(index.get_url_suffixes('-Wno-alloca'),
                         None)
        self.assertEqual(index.get_url_suffixes('-Walloca'),
                         {'gcc/Warning-Options.html#index-Wno-alloca'})

    def test_parse_option_index(self):
        index = Index()
        index.parse_option_index(INPUT_HTML_PATH / 'gcc/Option-Index.html')
        self.assertEqual(index.get_url_suffixes('-fmodulo-sched'),
                         {'gcc/Optimize-Options.html#index-fmodulo-sched'})
        self.assertEqual(index.get_url_suffixes('-O'),
                         {'gcc/Optimize-Options.html#index-O'})
        self.assertEqual(index.get_url_suffixes('-O0'),
                         {'gcc/Optimize-Options.html#index-O0'})
        self.assertEqual(index.get_url_suffixes('-Wframe-larger-than='),
                         {'gcc/Warning-Options.html#index-Wframe-larger-than_003d'})

        # Check an option with duplicates: '-march'
        # The url_suffixes will be of the form
        #  'gcc/HPPA-Options.html#index-march-5',
        #  'gcc/LoongArch-Options.html#index-march-7',
        # etc, where the trailing number is, unfortunately, likely to
        # change from release to release.
        # Replace them with 'NN' for the purpose of this test:
        em_arch_url_suffixes = [re.sub('(-[0-9]+)', '-NN', s)
                                for s in index.get_url_suffixes('-march')]
        if 0:
            print(em_arch_url_suffixes)
        self.assertIn('gcc/ARM-Options.html#index-march-NN', em_arch_url_suffixes)
        self.assertIn('gcc/x86-Options.html#index-march-NN', em_arch_url_suffixes)

        self.assertEqual(index.get_url_suffixes('-Wcpp'),
                         {'gcc/Warning-Options.html#index-Wcpp'})

        self.assertNotEqual(index.get_url_suffixes('-march'), None)
        self.assertNotEqual(index.get_url_suffixes('-march='), None)

class OptFile:
    def __init__(self, opt_path, rel_path):
        """
        Parse a .opt file.  Similar to opt-gather.awk.
        """
        self.rel_path = rel_path
        assert rel_path.startswith('gcc')
        # self.filename = os.path.basename(path)
        self.records = []
        with open(opt_path) as f:
            flag = 0
            for line in f:
                #print(repr(line))
                if re.match(r'[ \t]*(;|$)', line):
                    flag = 0
                else:
                    if flag == 0:
                        self.records.append([line])
                        flag = 1
                    else:
                        self.records[-1].append(line)

# Mapping from target-specific page to subdirectory containing .opt files
# documented on that page.

TARGET_SPECIFIC_PAGES = {
    'gcc/AArch64-Options.html' : 'gcc/config/aarch64/',
    'gcc/AMD-GCN-Options.html' : 'gcc/config/gcn/',
    'gcc/ARC-Options.html' : 'gcc/config/arc/',
    'gcc/ARC-Options.html' : 'gcc/config/arc/',
    'gcc/ARM-Options.html' : 'gcc/config/arm/',
    'gcc/AVR-Options.html' : 'gcc/config/avr/',
    'gcc/Adapteva-Epiphany-Options.html' : 'gcc/config/epiphany/',
    'gcc/Blackfin-Options.html' : 'gcc/config/bfin/',
    'gcc/C-SKY-Options.html' : 'gcc/config/csky/',
    'gcc/C6X-Options.html' : 'gcc/config/c6x/',
    'gcc/CRIS-Options.html' : 'gcc/config/cris/',
    'gcc/DEC-Alpha-Options.html' : 'gcc/config/alpha/',
    'gcc/FR30-Options.html' : 'gcc/config/fr30/',
    'gcc/FRV-Options.html' : 'gcc/config/frv/',
    'gcc/FT32-Options.html' : 'gcc/config/ft32/',
    'gcc/H8_002f300-Options.html' : 'gcc/config/h8300/',
    'gcc/HPPA-Options.html' : 'gcc/config/pa/',
    'gcc/IA-64-Options.html' : 'gcc/config/ia64/',
    'gcc/LoongArch-Options.html' : 'gcc/config/loongarch/',
    'gcc/M32C-Options.html' : 'gcc/config/m32c/',
    'gcc/M32R_002fD-Options.html' : 'gcc/config/m32r/',
    'gcc/M680x0-Options.html' : 'gcc/config/m68k/',
    'gcc/MCore-Options.html' : 'gcc/config/mcore/',
    'gcc/MIPS-Options.html' : 'gcc/config/mips/',
    'gcc/MMIX-Options.html' : 'gcc/config/mmix/',
    'gcc/MN10300-Options.html' : 'gcc/config/mn10300/',
    'gcc/MSP430-Options.html' : 'gcc/config/msp430/',
    'gcc/MicroBlaze-Options.html' : 'gcc/config/microblaze/',
    'gcc/Moxie-Options.html' : 'gcc/config/moxie/',
    'gcc/NDS32-Options.html' : 'gcc/config/nds32/',
    'gcc/Nios-II-Options.html' : 'gcc/config/nios2/',
    'gcc/Nvidia-PTX-Options.html' : 'gcc/config/nvptx/',
    'gcc/OpenRISC-Options.html' : 'gcc/config/or1k/',
    'gcc/PDP-11-Options.html' : 'gcc/config/pdp11',
    'gcc/PRU-Options.html' : 'gcc/config/pru/',
    'gcc/RISC-V-Options.html' : 'gcc/config/riscv/',
    'gcc/RL78-Options.html' : 'gcc/config/rl78/',
    'gcc/RS_002f6000-and-PowerPC-Options.html' : 'gcc/config/rs6000/',
    'gcc/RX-Options.html' : 'gcc/config/rx/',
    'gcc/SH-Options.html' : 'gcc/config/sh/',
    'gcc/SPARC-Options.html' : 'gcc/config/sparc/',
    'gcc/S_002f390-and-zSeries-Options.html' : 'gcc/config/s390',
    'gcc/V850-Options.html' : 'gcc/config/vax/',
    'gcc/VAX-Options.html' : 'gcc/config/v850/',
    'gcc/Visium-Options.html' : 'gcc/config/visium/',
    'gcc/Xstormy16-Options.html' : 'gcc/config/stormy16/',
    'gcc/Xtensa-Options.html' : 'gcc/config/xtensa/',
    'gcc/eBPF-Options.html' : 'gcc/config/bpf/',
    'gcc/x86-Options.html' : 'gcc/config/i386/',
}

def target_specific(url_suffix):
    for page_prefix, subdir in TARGET_SPECIFIC_PAGES.items():
        if url_suffix.startswith(page_prefix):
            return subdir

def filter_urlsuffixes_for_optfile(optfile, url_suffixes):
    """
    Filter out target-specific options for the wrong target.
    """
    result = set()
    for url_suffix in url_suffixes:
        subdir = target_specific(url_suffix)
        if subdir:
            if 0:
                print(f'{optfile.rel_path=}')
                print(f'{url_suffixes=}')
                print(f'{subdir=}')
            if not optfile.rel_path.startswith(subdir):
                # Skip this
                continue
        result.add(url_suffix)
    return result


class TestFiltering(unittest.TestCase):
    def test_target_specific(self):
        self.assertEqual(target_specific('gcc/Preprocessor-Options.html#index-A'),
                         None)
        self.assertEqual(target_specific('gcc/MMIX-Options.html#index-mknuthdiv'),
                         'gcc/config/mmix/')

    def test_filter(self):
        s = {'gcc/MIPS-Options.html#index-munaligned-access-1',
             'gcc/ARM-Options.html#index-munaligned-access'}
        arm_optfile = OptFile('/dev/null', 'gcc/config/arm/arm.opt')
        mips_optfile = OptFile('/dev/null', 'gcc/config/mips/mips.opt')
        self.assertEqual(
            filter_urlsuffixes_for_optfile(arm_optfile, s),
            {'gcc/ARM-Options.html#index-munaligned-access'})
        self.assertEqual(
            filter_urlsuffixes_for_optfile(mips_optfile, s),
            {'gcc/MIPS-Options.html#index-munaligned-access-1'})


def write_url_file(index, optfile, dstfile):
    dstfile.write('; Autogenerated by regenerate-opt-urls.py from %s'
                  ' and generated HTML\n\n'
                  % optfile.rel_path)
    for record in optfile.records:
        opt = '-' + record[0].strip()
        if 0:
            dstfile.write('; entry for %s\n' % record)
            dstfile.write('; opt=%r\n' % opt)
        url_suffixes = index.get_url_suffixes(opt)
        if 0:
            dstfile.write('; url_suffixes=%r\n' % url_suffixes)
        if url_suffixes:
            url_suffixes = filter_urlsuffixes_for_optfile(optfile, url_suffixes)
            if url_suffixes:
                if len(url_suffixes) == 1:
                    dstfile.write('%s\n' % opt[1:])
                    dstfile.write('UrlSuffix(%s)\n' % list(url_suffixes)[0])
                else:
                    dstfile.write('; skipping %r due to multiple URLs:\n'
                                  % opt[1:])
                    for u in sorted(url_suffixes):
                        dstfile.write(';   duplicate: %r\n' % u)
            else:
                dstfile.write('; skipping %r due to finding no URLs\n'
                              % opt[1:])
            dstfile.write('\n')


def main(args):
    index = Index()
    index.parse_option_index(args.base_html_dir / 'gcc/Option-Index.html')
    if 0:
        pprint(index.entries)
    for root, dirs, files in os.walk(args.src_gcc_dir):
        for f in files:
            if f.endswith('.opt'):
                opt_path = os.path.join(root, f)
                rel_path = os.path.relpath(opt_path, args.src_gcc_dir)
                optfile = OptFile(opt_path, rel_path)
                if 0:
                    pprint(optfile.path)
                    pprint(optfile.records)
                dstname = f + '.urls'
                urlfile = os.path.join(root, dstname)
                with open(urlfile, 'w') as dstfile:
                    write_url_file(index, optfile, dstfile)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=DESCRIPTION,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('base_html_dir', type=Path)
    parser.add_argument('src_gcc_dir', type=Path)
    parser.add_argument('--unit-test', action='store_true')
    args = parser.parse_args()

    if args.unit_test:
        INPUT_HTML_PATH = args.base_html_dir
        unittest.main(argv=[sys.argv[0], '-v'])
    else:
        main(args)
