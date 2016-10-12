/*
 * Copyright (c) 2016, Egor Pugin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of
 *        its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "common.h"

#include <regex>

using ProjectVersionId = uint64_t;
using ProjectVersionNumber = int32_t;

extern const std::regex r_branch_name;

struct Version
{
    // undef gcc symbols
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif
#ifdef patch
#undef patch
#endif

    ProjectVersionNumber major = -1;
    ProjectVersionNumber minor = -1;
    ProjectVersionNumber patch = -1;
    String branch;

    Version(ProjectVersionNumber ma = -1, ProjectVersionNumber mi = -1, ProjectVersionNumber pa = -1);
    Version(const String &s);

    String toAnyVersion() const;
    String toString() const;
    path toPath() const;

    bool isValid() const;
    bool isBranch() const { return !branch.empty(); }
    bool isVersion() const { return !isBranch(); }

    // checks if this version can be rhs using upgrade rules
    // does not check branches!
    // rhs should be exact version
    bool canBe(const Version &rhs) const;

    bool operator<(const Version &rhs) const;
    bool operator==(const Version &rhs) const;
    bool operator!=(const Version &rhs) const;
};
