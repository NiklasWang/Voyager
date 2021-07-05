#include "Common.h"
#include "InterfaceEx.h"

namespace pandora {

int32_t Selection::size() const
{
    return list.size();
}

int32_t Selection::add(const AlgorithmInf &inf)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (inf.name    == "" &&
            inf.author  == "" &&
            inf.version < 0) {
            LOGE(MODULE_COMMON, "Invalid algorithm information: %s %s %d",
                inf.name.c_str(), inf.author.c_str(), inf.version);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        for (auto &&alg : list) {
            if (alg.name    == inf.name   &&
                alg.author  == inf.author &&
                alg.version == inf.version) {
                LOGE(MODULE_COMMON, "Algorithm information already in set, %s %s %d",
                    inf.name.c_str(), inf.author.c_str(), inf.version);
                    rc = ALREADY_EXISTS;
            }
        }
    }

    if (SUCCEED(rc)) {
        list.push_back(inf);
        if (!valid()) {
            list.erase(list.end()--);
            LOGE(MODULE_COMMON, "Algorithm %s %s %d makes set invalid.",
                inf.name.c_str(), inf.author.c_str(), inf.version);
            dump();
            rc = PARAM_INVALID;
        }
    }

    return rc;
}

int32_t Selection::remove(const AlgorithmInf &inf)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = NOT_FOUND;
        auto alg = list.begin();
        while (alg != list.end()) {
            if (alg->name    == inf.name   &&
                alg->author  == inf.author &&
                alg->version == inf.version) {
                alg = list.erase(alg);
                rc = NO_ERROR;
            } else {
                alg++;
            }
        }
    }

    if (FAILED(rc)) {
        LOGE(MODULE_COMMON, "Not found algorithm %s %s %d information in set.",
            inf.name.c_str(), inf.author.c_str(), inf.version);
    }

    return rc;
}

bool Selection::valid() const
{
    int32_t rc = NO_ERROR;
    bool result = true;
    bool selectFirst = false;

    if (SUCCEED(rc)) {
        if (size() > 0) {
            const AlgorithmInf &inf = *list.begin();
            selectFirst = inf.select;
        }
    }

    if (SUCCEED(rc)) {
        for (auto alg : list) {
            if (selectFirst != alg.select) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Selection::isSelect() const
{
    bool result = false;

    if (valid()) {
        if (size() > 0) {
            const AlgorithmInf &inf = *list.begin();
            result = inf.select;
        }
    }

    return result;
}

bool Selection::isDeselect() const
{
    bool result = false;

    if (valid()) {
        if (size() > 0) {
            const AlgorithmInf &inf = *list.begin();
            result = !inf.select;
        }
    }

    return result;
}

void Selection::dump() const
{
    LOGI(MODULE_COMMON, "Dump algorithm set, %d total :", size());
    for (auto alg : list) {
        LOGI(MODULE_COMMON, "  - Algorithm %s %s %d, %s.",
            alg.name.c_str(), alg.author.c_str(), alg.version,
            alg.select ? "selected" : "deselected");
    }
    LOGI(MODULE_COMMON, "Dump algorithm finished.");
}

};
