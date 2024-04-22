/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MU_PROJECT_NOTATIONPROJECT_H
#define MU_PROJECT_NOTATIONPROJECT_H

#include "../inotationproject.h"

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "io/ifilesystem.h"
#include "../iprojectconfiguration.h"
#include "inotationreadersregister.h"
#include "inotationwritersregister.h"

#include "engraving/engravingproject.h"

#include "notation/inotationcreator.h"
#include "notation/inotationconfiguration.h"
#include "projectaudiosettings.h"
#include "iprojectmigrator.h"

namespace mu::engraving {
class MscReader;
class MscWriter;
}

namespace mu::project {
class NotationProject : public INotationProject, public muse::async::Asyncable
{
    INJECT(muse::io::IFileSystem, fileSystem)
    INJECT(IProjectConfiguration, configuration)
    INJECT(notation::INotationConfiguration, notationConfiguration)
    INJECT(notation::INotationCreator, notationCreator)
    INJECT(INotationReadersRegister, readers)
    INJECT(INotationWritersRegister, writers)
    INJECT(IProjectMigrator, migrator)

public:
    ~NotationProject() override;

    muse::Ret load(const muse::io::path_t& path,
                   const muse::io::path_t& stylePath = muse::io::path_t(), bool forceMode = false, const std::string& format = "") override;
    muse::Ret createNew(const ProjectCreateOptions& projectInfo) override;

    muse::io::path_t path() const override;
    void setPath(const muse::io::path_t& path) override;
    muse::async::Notification pathChanged() const override;

    QString displayName() const override;
    muse::async::Notification displayNameChanged() const override;

    bool isCloudProject() const override;
    const CloudProjectInfo& cloudInfo() const override;
    void setCloudInfo(const CloudProjectInfo& info) override;

    const CloudAudioInfo& cloudAudioInfo() const override;
    void setCloudAudioInfo(const CloudAudioInfo& audioInfo) override;

    bool isNewlyCreated() const override;
    void markAsNewlyCreated() override;

    bool isImported() const override;

    void markAsUnsaved() override;

    muse::ValNt<bool> needSave() const override;
    muse::Ret canSave() const override;

    bool needAutoSave() const override;
    void setNeedAutoSave(bool val) override;

    muse::Ret save(const muse::io::path_t& path = muse::io::path_t(), SaveMode saveMode = SaveMode::Save) override;
    muse::Ret writeToDevice(QIODevice* device) override;

    ProjectMeta metaInfo() const override;
    void setMetaInfo(const ProjectMeta& meta, bool undoable = false) override;

    notation::IMasterNotationPtr masterNotation() const override;
    IProjectAudioSettingsPtr audioSettings() const override;

private:
    void setupProject();

    muse::Ret loadTemplate(const ProjectCreateOptions& projectOptions);

    muse::Ret doLoad(const muse::io::path_t& path, const muse::io::path_t& stylePath, bool forceMode, const std::string& format);
    muse::Ret doImport(const muse::io::path_t& path, const muse::io::path_t& stylePath, bool forceMode);

    muse::Ret saveScore(const muse::io::path_t& path, const std::string& fileSuffix, bool generateBackup = true,
                        bool createThumbnail = true);
    muse::Ret saveSelectionOnScore(const muse::io::path_t& path = muse::io::path_t());
    muse::Ret exportProject(const muse::io::path_t& path, const std::string& suffix);
    muse::Ret doSave(const muse::io::path_t& path, engraving::MscIoMode ioMode, bool generateBackup = true, bool createThumbnail = true);
    muse::Ret makeCurrentFileAsBackup();
    muse::Ret writeProject(engraving::MscWriter& msczWriter, bool onlySelection, bool createThumbnail = true);

    void listenIfNeedSaveChanges();
    void markAsSaved(const muse::io::path_t& path);
    void setNeedSave(bool needSave);

    mu::engraving::EngravingProjectPtr m_engravingProject = nullptr;
    notation::IMasterNotationPtr m_masterNotation = nullptr;
    ProjectAudioSettingsPtr m_projectAudioSettings = nullptr;
    mutable CloudProjectInfo m_cloudInfo;
    mutable CloudAudioInfo m_cloudAudioInfo;

    muse::io::path_t m_path;
    muse::async::Notification m_pathChanged;
    muse::async::Notification m_displayNameChanged;

    muse::async::Notification m_needSaveNotification;

    bool m_isNewlyCreated = false; /// true if the file has never been saved yet
    bool m_isImported = false;
    bool m_needAutoSave = false;
    bool m_hasNonUndoStackChanges = false;
};
}

#endif // MU_PROJECT_NOTATIONPROJECT_H
