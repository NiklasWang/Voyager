#include <string>
#include <list>
#include <iostream>

enum err {
    NO_ERROR,
    JUMP_DONE,
};

#define SUCCEED(rc) ((rc) == NO_ERROR)
#define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

struct Node {
    std::string     name;
    std::list<Node> subNodes;
};

struct Info {
    std::string name;
    int32_t     subNodeNum;
};

std::list<Node> buildTreeNodes(std::list<Info> infos, int32_t copies)
{
    int32_t rc = NO_ERROR;
    std::list<Info> subInfos;
    Info    currInfo;
    int32_t subNodeNum = 1;
    std::list<Node> result;

    if (SUCCEED(rc)) {
        if (infos.size() == 0) {
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        subInfos = infos;
        currInfo = *(subInfos.begin());
        subNodeNum = currInfo.subNodeNum;
        subInfos.erase(subInfos.begin());
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < copies; i++) {
            Node node;
            node.name = currInfo.name;
            std::list<Node> subNodes = buildTreeNodes(subInfos, subNodeNum);
            if (subNodes.size() == subNodeNum) {
                node.subNodes = subNodes;
            } else if (!subNodes.size() && !subInfos.size()) {
                // last one
            } else {
                std::cout << "Wrong number of sub nodes." << std::endl;
            }
            result.push_back(node);
        }
    }

    return result;
}

std::list<Node> buildTreeNodes(std::list<Info> &infos)
{
    return buildTreeNodes(infos, 1);
}

int32_t traverseTreeNode(
    const Node parentNode,
    const std::list<Node> treeNode,
    std::list<std::list<std::string> > &lists,
    std::list<std::string> &list)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (treeNode.size() == 0) {
            lists.push_back(list);
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        for (auto itr = treeNode.begin(); itr != treeNode.end(); itr++) {
            if (itr == treeNode.begin()) {
                list.push_back(itr->name + std::to_string(itr->subNodes.size()));
                rc = traverseTreeNode(*itr, itr->subNodes, lists, list);
            } else {
                std::list<std::string> newlist;
                newlist.push_back(parentNode.name + std::to_string(parentNode.subNodes.size()));
                newlist.push_back(itr->name + std::to_string(itr->subNodes.size()));
                rc = traverseTreeNode(*itr, itr->subNodes, lists, newlist);
            }
        }
    }

    return rc;
}

int32_t traverseTreeNode(
    const std::list<Node> tree,
    std::list<std::list<std::string> > &lists)
{
    std::list<std::string> firstList;
    int32_t rc = traverseTreeNode(*tree.begin(), tree, lists, firstList);
    return rc == JUMP_DONE ? NO_ERROR : rc;
}

int32_t printTreeNodes(const Node &node, std::string &name)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        name += node.name + std::to_string(node.subNodes.size());
        if (node.subNodes.size() != 0) {
            name += " -> ";
        }
        for (auto &&subNode : node.subNodes) {
            rc = printTreeNodes(subNode, name);
            if (!SUCCEED(rc)) {
                std::cout << "Failed to traverse sub tree nodes, " << rc << std::endl;
            }
        }
        if (node.subNodes.size() == 0) {
            name += "; \n";
        }
    }

    return rc;
}

int32_t buildAndPrintTreeNodes(std::list<Info> infos)
{
    int32_t rc = NO_ERROR;
    std::list<Node> tree;

    if (SUCCEED(rc)) {
        tree = buildTreeNodes(infos);
        if (tree.size() == 0) {
            std::cout << "Build tree nodes failed, " << rc << std::endl;
        }
    }

    if (SUCCEED(rc)) {
        std::string text;
        std::cout << "Now Print Tree Nodes:" << std::endl;
        rc = printTreeNodes(*tree.begin(), text);
        if (SUCCEED(rc)) {
            std::cout << text << std::endl;
        } else {
            std::cout << "Failed, " << rc << std::endl;
        }
    }

    return rc;
}

#define PLACE_HOLDER_NAME "place_holder"

int32_t buildAndTraverseTreeNodes(std::list<Info> infos)
{
    int32_t rc = NO_ERROR;
    std::list<Info> infoList = infos;
    std::list<Node> tree;
    std::list<std::list<std::string> > lists;

    if (SUCCEED(rc)) {
        infoList.push_back({
            .name = PLACE_HOLDER_NAME,
            .subNodeNum = 1,
        });
    }

    if (SUCCEED(rc)) {
        tree = buildTreeNodes(infoList);
        if (tree.size() == 0) {
            std::cout << "Build tree nodes failed, " << rc << std::endl;
        }
    }

    if (SUCCEED(rc)) {
        std::cout << "Now Traverse Tree Nodes:" << std::endl;
        rc = traverseTreeNode(tree, lists);
        if (!SUCCEED(rc)) {
            std::cout << "Failed, " << rc << std::endl;
        }
    }

    if (SUCCEED(rc)) {
        for (auto &&list : lists) {
            auto itr = list.begin();
            while (itr != list.end()) {
                if ((*itr).find(PLACE_HOLDER_NAME) != std::string::npos) {
                    itr = list.erase(itr);
                } else {
                    itr++;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        for (auto &&list : lists) {
            bool first = true;
            for (auto &&name : list) {
                if (!first) {
                    std::cout << " -> ";
                }
                std::cout << name;
                first = false;
            }
            std::cout << std::endl;
        }
    }

    return rc;
}

int32_t runTestCase(const Info *infoArray, uint32_t size)
{
    int32_t rc = NO_ERROR;
    std::list<Info> infos;

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < size; i++) {
            infos.push_back(infoArray[i]);
        }
    }

    if (SUCCEED(rc)) {
        std::string names;
        bool first = true;
        for (auto &&info : infos) {
            if (!first) {
                names += " -> ";
            }
            names += info.name;
            names += std::to_string(info.subNodeNum);
            first = false;
        }
        std::cout << "============================================" << std::endl;
        std::cout << names << std::endl << std::endl;
    }

    if (SUCCEED(rc)) {
        rc = buildAndPrintTreeNodes(infos);
        if (!SUCCEED(rc)) {
            std::cout << "Failed to build and print, " << rc << std::endl;
        }
    }

    if (SUCCEED(rc)) {
        rc = buildAndTraverseTreeNodes(infos);
        if (!SUCCEED(rc)) {
            std::cout << "Failed to build and traverse, " << rc << std::endl;
        }
    }

    return rc;
}

static const Info infos1Single[] = {
    {
        .name = "A",
        .subNodeNum = 1,
    },
};

static const Info infos2Single[] = {
    {
        .name = "A",
        .subNodeNum = 1,
    },
    {
        .name = "B",
        .subNodeNum = 1,
    },
};

static const Info infos4Single[] = {
    {
        .name = "A",
        .subNodeNum = 1,
    },
    {
        .name = "B",
        .subNodeNum = 1,
    },
    {
        .name = "C",
        .subNodeNum = 1,
    },
    {
        .name = "D",
        .subNodeNum = 1,
    },
};

static const Info infos1Single1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 3,
    },
    {
        .name = "B",
        .subNodeNum = 1,
    },
};

static const Info infos1x2Multiple1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 3,
    },
    {
        .name = "B",
        .subNodeNum = 2,
    },
};

static const Info infos1x2Multiple2x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 3,
    },
    {
        .name = "B",
        .subNodeNum = 2,
    },
    {
        .name = "C",
        .subNodeNum = 3,
    },
};

static const Info infos1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 3,
    },
};

static const Info infos2Single1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 1,
    },
    {
        .name = "B",
        .subNodeNum = 3,
    },
    {
        .name = "C",
        .subNodeNum = 1,
    },
};


static const Info infos2x2Multiple1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 2,
    },
    {
        .name = "B",
        .subNodeNum = 3,
    },
    {
        .name = "C",
        .subNodeNum = 2,
    },
};

static const Info infos3Single1x2Multiple1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 1,
    },
    {
        .name = "B",
        .subNodeNum = 3,
    },
    {
        .name = "C",
        .subNodeNum = 1,
    },
    {
        .name = "D",
        .subNodeNum = 2,
    },
    {
        .name = "E",
        .subNodeNum = 1,
    },
};

static const Info infos5Single2x2Multiple1x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 1,
    },
    {
        .name = "B",
        .subNodeNum = 2,
    },
    {
        .name = "C",
        .subNodeNum = 1,
    },
    {
        .name = "D",
        .subNodeNum = 3,
    },
    {
        .name = "E",
        .subNodeNum = 1,
    },
    {
        .name = "F",
        .subNodeNum = 1,
    },
    {
        .name = "G",
        .subNodeNum = 2,
    },
    {
        .name = "H",
        .subNodeNum = 1,
    },
};

static const Info infos5Single1x2Multiple2x3Multiple[] = {
    {
        .name = "A",
        .subNodeNum = 2,
    },
    {
        .name = "B",
        .subNodeNum = 1,
    },
    {
        .name = "C",
        .subNodeNum = 1,
    },
    {
        .name = "D",
        .subNodeNum = 3,
    },
    {
        .name = "E",
        .subNodeNum = 1,
    },
    {
        .name = "F",
        .subNodeNum = 1,
    },
    {
        .name = "G",
        .subNodeNum = 3,
    },
    {
        .name = "H",
        .subNodeNum = 1,
    },
};

int32_t main(int argc,char *argv[])
{
    int32_t rc = NO_ERROR;

    rc = rc || runTestCase(infos1Single, ARRAYSIZE(infos1Single));
    rc = rc || runTestCase(infos2Single, ARRAYSIZE(infos2Single));
    rc = rc || runTestCase(infos4Single, ARRAYSIZE(infos4Single));
    rc = rc || runTestCase(infos1Single1x3Multiple,     ARRAYSIZE(infos1Single1x3Multiple));
    rc = rc || runTestCase(infos1x2Multiple1x3Multiple, ARRAYSIZE(infos1x2Multiple1x3Multiple));
    rc = rc || runTestCase(infos1x2Multiple2x3Multiple, ARRAYSIZE(infos1x2Multiple2x3Multiple));
    rc = rc || runTestCase(infos1x3Multiple,                   ARRAYSIZE(infos1x3Multiple));
    rc = rc || runTestCase(infos2Single1x3Multiple,            ARRAYSIZE(infos2Single1x3Multiple));
    rc = rc || runTestCase(infos2x2Multiple1x3Multiple,        ARRAYSIZE(infos2x2Multiple1x3Multiple));
    rc = rc || runTestCase(infos3Single1x2Multiple1x3Multiple, ARRAYSIZE(infos3Single1x2Multiple1x3Multiple));
    rc = rc || runTestCase(infos5Single2x2Multiple1x3Multiple, ARRAYSIZE(infos5Single2x2Multiple1x3Multiple));
    rc = rc || runTestCase(infos5Single1x2Multiple2x3Multiple, ARRAYSIZE(infos5Single1x2Multiple2x3Multiple));

    return rc;
}

