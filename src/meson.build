# This file is part of the miniyaml Library
# Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
# 		      alternatives
#
# SPDX-License-Identifer: LGPL-3.0-or-later
#
# author: Quentin Bouget <quentin.bouget@cea.fr>

libminiyaml = library(
	'miniyaml',
	sources: [
		'miniyaml.c',
		'base64.c',
	],
	version: meson.project_version(),
    dependencies: [libyaml],
	include_directories: include_dirs,
	install: true,
)

# Build a .pc file
pkg_mod = import('pkgconfig')
pkg_mod.generate(libminiyaml,
		 description: 'dev-friendly wrappers around libyaml',
		 requires: libyaml.name())
