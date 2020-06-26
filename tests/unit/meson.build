# This file is part of the RobinHood Library
# Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
#                    alternatives
#
# SPDX-License-Identifer: LGPL-3.0-or-later

foreach t: ['check_skip']
    test(t, executable(t, t + '.c',
                       dependencies: [check, libyaml],
                       link_with: [libminiyaml],
                       include_directories: include_dirs))
endforeach
