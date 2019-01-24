//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/common/ModuleAccess.h"
#include "inet/common/OSGScene.h"
#include "inet/common/OSGUtils.h"
#include "inet/visualizer/mobility/MobilityOsgVisualizer.h"

#ifdef WITH_OSG
#include <osg/AutoTransform>
#include <osg/Material>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgText/Text>
#endif // ifdef WITH_OSG

namespace inet {

namespace visualizer {

Define_Module(MobilityOsgVisualizer);

#ifdef WITH_OSG

MobilityOsgVisualizer::CacheEntry::CacheEntry(NetworkNodeOsgVisualization *networkNode, osg::Geode *trail) :
    networkNode(networkNode),
    trail(trail)
{
}

void MobilityOsgVisualizer::initialize(int stage)
{
    MobilityVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL)
        networkNodeVisualizer = getModuleFromPar<NetworkNodeOsgVisualizer>(par("networkNodeVisualizerModule"), this);
}

MobilityOsgVisualizer::CacheEntry *MobilityOsgVisualizer::getCacheEntry(const IMobility *mobility) const
{
    auto it = cacheEntries.find(mobility);
    if (it == cacheEntries.end())
        return nullptr;
    else
        return it->second;
}

void MobilityOsgVisualizer::setCacheEntry(const IMobility *mobility, CacheEntry *entry)
{
    cacheEntries[mobility] = entry;
}

void MobilityOsgVisualizer::removeCacheEntry(const IMobility *mobility)
{
    cacheEntries.erase(mobility);
}

MobilityOsgVisualizer::CacheEntry* MobilityOsgVisualizer::ensureCacheEntry(const IMobility *mobility)
{
    auto cacheEntry = getCacheEntry(mobility);
    if (cacheEntry == nullptr) {
        auto module = const_cast<cModule *>(check_and_cast<const cModule *>(mobility));
        auto trail = new osg::Geode();
        cFigure::Color color = cFigure::GOOD_DARK_COLORS[module->getId() % (sizeof(cFigure::GOOD_DARK_COLORS) / sizeof(cFigure::Color))];
        trail->setStateSet(inet::osg::createStateSet(color, 1.0));
        auto networkNode = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(module));
        auto scene = inet::osg::TopLevelScene::getSimulationScene(visualizerTargetModule);
        scene->addChild(trail);
        cacheEntry = new CacheEntry(networkNode, trail);
        setCacheEntry(mobility, cacheEntry);
    }
    return cacheEntry;
}

void MobilityOsgVisualizer::extendMovementTrail(osg::Geode *trail, const Coord& position)
{
    if (trail->getNumDrawables() == 0)
        trail->addDrawable(inet::osg::createLineGeometry(position, position));
    else {
        auto drawable = static_cast<osg::Geometry *>(trail->getDrawable(0));
        auto vertices = static_cast<osg::Vec3Array *>(drawable->getVertexArray());
        auto lastPosition = vertices->at(vertices->size() - 1);
        auto dx = lastPosition.x() - position.x;
        auto dy = lastPosition.y() - position.y;
        auto dz = lastPosition.z() - position.z;
        // TODO: 1?
        if (dx * dx + dy * dy + dz * dz > 1) {
            vertices->push_back(osg::Vec3d(position.x, position.y, position.z));
            if (vertices->size() > trailLength)
                vertices->erase(vertices->begin(), vertices->begin() + 1);
            auto drawArrays = static_cast<osg::DrawArrays *>(drawable->getPrimitiveSet(0));
            drawArrays->setFirst(0);
            drawArrays->setCount(vertices->size());
            drawable->dirtyBound();
            drawable->dirtyDisplayList();
        }
    }
}

void MobilityOsgVisualizer::receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG)
{
    Enter_Method_Silent();
    if (!hasGUI()) return;
    if (signal == IMobility::mobilityStateChangedSignal) {
        auto mobility = dynamic_cast<IMobility *>(object);
        auto position = mobility->getCurrentPosition();
        auto orientation = mobility->getCurrentAngularPosition();
        auto cacheEntry = ensureCacheEntry(mobility);
        auto networkNode = cacheEntry->networkNode;
        networkNode->setPosition(osg::Vec3d(position.x, position.y, position.z));
        networkNode->setAttitude(osg::Quat(orientation.alpha, osg::Vec3d(0.0, 0.0, 1.0)) *
                                 osg::Quat(orientation.beta, osg::Vec3d(0.0, 1.0, 0.0)) *
                                 osg::Quat(orientation.gamma, osg::Vec3d(1.0, 0.0, 0.0)));
        if (displayMovementTrail)
            extendMovementTrail(cacheEntry->trail, position);
    }
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

