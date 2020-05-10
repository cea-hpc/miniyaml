# This file is part of the miniyaml Library
# Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
# 		      alternatives
#
# SPDX-License-Identifer: LGPL-3.0-or-later
#
# author: Quentin Bouget <quentin.bouget@cea.fr>

executable(
	'emitter',
	sources: ['emitter.c'],
	dependencies: [libyaml],
	include_directories: include_dirs,
	link_with: libminiyaml,
	install: false,
)

executable(
	'parser',
	sources: ['parser.c'],
	dependencies: [libyaml],
	include_directories: include_dirs,
	link_with: libminiyaml,
	install: false,
)
