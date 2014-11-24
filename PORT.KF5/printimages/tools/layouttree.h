/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-13
 * Description : Layouting photos on a page
 *
 * Copyright 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef LAYOUTTREE_H
#define LAYOUTTREE_H

// Qt includes

#include <QRectF>
#include <QMap>

/**
    Implements the algorithm described in

    "Adaptive Photo Collection Page Layout",
    C. Brian Atkins
    Imaging Technology Department
    HP Labs
    Palo Alto, CA 94304
    cbatkins@hpl.hp.com

    PDF available at:
    http://hpl.hp.com/research/isl/layout/

    Comments in the source file refer to the PDF file.
*/

/**
    The classes LayoutNode and LayoutTree provide the actual implementation.
    Do not use these classes directly.
    Use the AtkinsPageLayout adaptor class.
*/

namespace KIPIPrintImagesPlugin
{

class LayoutNode
{
public:

    enum Type
    {
        TerminalNode,
        HorizontalDivision, // one image on top of the other
        VerticalDivision    // images side by side
    };

public:

    LayoutNode(double aspectRatio, double relativeArea, int index);
    LayoutNode(LayoutNode* const subtree, LayoutNode* const terminalChild, bool horizontal, int index);
    LayoutNode(const LayoutNode&);
    ~LayoutNode();

    double aspectRatio()  const { return m_a;        }
    double relativeArea() const { return m_e;        }
    double division()     const { return m_division; }

    Type type() const { return m_type;  }
    int index() const { return m_index; }

    LayoutNode* leftChild()  const { return m_leftChild;  }
    LayoutNode* rightChild() const { return m_rightChild; }

    void takeAndSetChild(LayoutNode* oldChild, LayoutNode* newChild);

    LayoutNode* nodeForIndex(int index);
    LayoutNode* parentOf(LayoutNode* child);

    void computeRelativeSizes();
    void computeDivisions();

    LayoutNode& operator=(const LayoutNode&);

private:

    double      m_a;
    double      m_e;
    double      m_division;

    Type        m_type;

    int         m_index;

    LayoutNode* m_leftChild;
    LayoutNode* m_rightChild;
};

// ---------------------------------------------------------------------

class LayoutTree
{
public:

    LayoutTree(double aspectRatioPage, double absoluteAreaPage);
    LayoutTree(const LayoutTree&);
    ~LayoutTree();

    int addImage(double aspectRatio, double relativeArea);
    QRectF drawingArea(int index, const QRectF& absoluteRectPage);

    int count() const;
    double score(LayoutNode* root, int nodeCount);
    double G() const;

    LayoutTree& operator=(const LayoutTree& other);

private:

    double absoluteArea(LayoutNode* node);
    QRectF rectInRect(const QRectF& rect, double aspectRatio, double absoluteArea);

private:

    LayoutNode* m_root;
    int         m_count;

    double      m_aspectRatioPage;
    double      m_absoluteAreaPage;
};

}  // NameSpace KIPIPrintImagesPlugin

#endif /* LAYOUTTREE_H */
