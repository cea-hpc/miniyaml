# This file is part of MiniYAML
# Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
# 		      alternatives
#
# SPDX-License-Identifer: LGPL-3.0-or-later

project(
	'miniyaml',
	'c',
	version: '0.0.0',
	license: 'LGPL3.0-or-later',
	default_options: [
		'werror=true',
	],
)

# Dependencies

libyaml = dependency('yaml-0.1', version: '>=0.1.7')

# Recurse in subdirectories

include_dirs = include_directories('include')
subdir('include')
subdir('src')
subdir('examples')
# TODO: subdir('tests/unit')
