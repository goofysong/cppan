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

#include "build.h"

#include <access_table.h>
#include <config.h>
#include <hash.h>
#include <http.h>
#include <resolver.h>

#include <iostream>

int build_packages(const String &name, const std::set<Package> &pkgs, const path &settings, const String &config);
int build_packages(const String &name, const std::set<Package> &pkgs, const Settings &settings);

int generate(path fn, const String &config)
{
    //auto conf = generate_config(fn, config);
    //return conf.settings.generate(&conf);
    return 0;
}

int build(path fn, const String &config)
{
    std::set<Package> pkgs;
    Config c;
    String name;
    std::tie(pkgs, c, name) = rd.read_packages_from_file(fn, config, true);
    return build_packages(name, pkgs, c.settings) == 0;
}

int build_only(path fn, const String &config)
{
    //AccessTable::do_not_update_files(true);

    //auto conf = generate_config(fn, config);
    //return conf.settings.build(&conf);
    return 0;
}

int dry_run(path p, const String &config)
{
    Config c(p);
    auto cppan_fn = p / CPPAN_FILENAME;
    //c.settings.prepare_build(&c, cppan_fn, read_file(cppan_fn));

    auto &project = c.getDefaultProject();
    auto dst = c.settings.source_directory / "src";
    for (auto &f : project.files)
    {
        fs::create_directories((dst / f).parent_path());
        fs::copy_file(p / f, dst / f, fs::copy_option::overwrite_if_exists);
    }
    fs::copy_file(p / CPPAN_FILENAME, dst / CPPAN_FILENAME, fs::copy_option::overwrite_if_exists);

    Config c2(dst);
    Package pkg;
    pkg.setLocalSourceDir(dst);

    // TODO: add more robust ppath, version choser
    pkg.flags.set(pfLocalProject);
    pkg.ppath = sha256(c.settings.source_directory.string());
    if (project.version.isValid())
        pkg.version = project.version;
    else
        pkg.version = String("master");

    c2.setPackage(pkg);
    rd.add_config(std::make_unique<Config>(c2), true);

    //auto conf = generate_config(p, config);
    //return conf.settings.build(&conf);
    return 0;
}

int build_packages(const String &name, const std::set<Package> &pkgs, const Settings &settings)
{
    Config c;
    c.settings = settings;
    for (auto &p : pkgs)
        c.getDefaultProject().addDependency(p);
    return c.settings.build_packages(c, name);
}

int build_packages(const String &name, const std::set<Package> &pkgs, const path &settings, const String &config)
{
    Config c;
    if (!settings.empty())
    {
        yaml root;
        auto s = YAML::LoadFile(settings.string());
        merge(s, root);

        if (!config.empty())
            root["local_settings"]["current_build"] = config;
        Config c2;
        c2.load(root);
        c.settings = c2.settings;
    }

    for (auto &p : pkgs)
        c.getDefaultProject().addDependency(p);
    return c.settings.build_packages(c, name);
}

int build_package(const String &target_name, const path &settings, const String &config)
{
    auto p = extractFromString(target_name);
    return build_packages(p.ppath.back(), { p }, settings, config);
}
