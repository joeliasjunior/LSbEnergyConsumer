//
// Copyright (C) 2016 OpenSim Ltd
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

#ifndef __INET_COUNTERFIGURE_H
#define __INET_COUNTERFIGURE_H

#include "IIndicatorFigure.h"
#include "inet/common/INETDefs.h"
#include "inet/common/INETMath.h"

// for the moment commented out as omnet cannot instatiate it from a namespace
//namespace inet {

#if OMNETPP_VERSION >= 0x500

class INET_API CounterFigure : public cGroupFigure, public inet::IIndicatorFigure
{
    struct Digit
    {
        cRectangleFigure *bounds;
        cTextFigure *text;

        Digit(cRectangleFigure *bounds, cTextFigure *text) : bounds(bounds), text(text) {}
    };

    cRectangleFigure *backgroundFigure;
    std::vector<Digit> digits;
    cTextFigure *labelFigure;

    double value = NaN;
    Anchor anchor = ANCHOR_NW;

  protected:
    virtual void parse(cProperty *property) override;
    virtual const char **getAllowedPropertyKeys() const override;
    cFigure::Point calculateRealPos(Point pos);
    void calculateBounds();
    void addChildren();
    void refresh();
    void layout();

  public:
    CounterFigure(const char *name = nullptr);
    virtual ~CounterFigure() {};

    virtual void setValue(int series, simtime_t timestamp, double value) override;

    // getters and setters
    cFigure::Color getBackgroundColor() const;
    void setBackgroundColor(cFigure::Color color);

    int getDecimalPlaces() const;
    void setDecimalPlaces(int radius);

    cFigure::Color getDigitBackgroundColor() const;
    void setDigitBackgroundColor(cFigure::Color color);

    cFigure::Color getDigitBorderColor() const;
    void setDigitBorderColor(cFigure::Color color);

    cFigure::Font getDigitFont() const;
    void setDigitFont(cFigure::Font font);

    cFigure::Color getDigitColor() const;
    void setDigitColor(cFigure::Color color);

    const char *getLabel() const;
    void setLabel(const char *text);

    cFigure::Font getLabelFont() const;
    void setLabelFont(cFigure::Font font);

    cFigure::Color getLabelColor() const;
    void setLabelColor(cFigure::Color color);

    Point getLabelPos() const;
    void setLabelPos(Point pos);

    Anchor getLabelAnchor() const;
    void setLabelAnchor(Anchor anchor);

    Point getPos() const;
    void setPos(Point bounds);

    Anchor getAnchor() const;
    void setAnchor(Anchor anchor);
};

#else

// dummy figure for OMNeT++ 4.x
class INET_API CounterFigure : public cGroupFigure {

};

#endif // omnetpp 5

// } // namespace inet

#endif

