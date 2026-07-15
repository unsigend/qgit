/* qgit - A simplified git like version control system
 * Copyright (C) 2025 - 2026 Qiu Yixiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <libqgit/error.h>
#include <libqgit/object/signature.h>
#include <libqgit/repo/config.h>
#include <libqgit/repo/repository.h>

int credentials_get(qgit_signature **out, qgit_repository *repo)
{
    qgit_config *local_config, *global_config = NULL;
    const char *name = NULL, *email = NULL;

    qgit_config_open_global(&global_config);
    local_config = qgit_repository_config(repo);

    qgit_config_get_string(&name, local_config, "user.name");
    if (!name && global_config)
        qgit_config_get_string(&name, global_config, "user.name");

    qgit_config_get_string(&email, local_config, "user.email");
    if (!email && global_config)
        qgit_config_get_string(&email, global_config, "user.email");

    if (!name || !email) {
        qgit_config_free(global_config);
        qgit_seterror(QGITERR_NOCREDENTIALS);
        return -1;
    }

    if (qgit_signature_now(out, name, email) < 0) {
        qgit_config_free(global_config);
        return -1;
    }

    qgit_config_free(global_config);
    return 0;
}