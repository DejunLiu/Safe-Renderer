/******************************************************************************
**
**   File:        ExpressionItemAtOperatorTest.cpp
**   Description:
**
**   Copyright (C) 2017 Luxoft GmbH
**
**   This file is part of Safe Renderer.
**
**   Safe Renderer is free software: you can redistribute it and/or
**   modify it under the terms of the GNU Lesser General Public
**   License as published by the Free Software Foundation.
**
**   Safe Renderer is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**   Lesser General Public License for more details.
**
**   You should have received a copy of the GNU Lesser General Public
**   License along with Safe Renderer.  If not, see
**   <http://www.gnu.org/licenses/>.
**
**   SPDX-License-Identifier: LGPL-3.0
**
******************************************************************************/

#include "ExpressionTestFixture.h"

#include <ExpressionOperators.h>

#include <gtest/gtest.h>

using namespace psc;

class ExpressionItemAtOpFixture : public ExpressionTestFixture
{
protected:
    ExpressionItemAtOpFixture()
    {
    }

    void addKeyValue(ExpressionTypeFactory& exprFactory, U32 value)
    {
        addIntegerValue(exprFactory, value);
    }

    void addWrongKeyValue(ExpressionTypeFactory& exprFactory, U32 value)
    {
        addWrongIntegerValue(exprFactory, value);
    }

    void addDefaultValue(ExpressionTypeFactory& exprFactory, U32 value)
    {
        addIntegerValue(exprFactory, value);
    }

    void addWrongDefaultValue(ExpressionTypeFactory& exprFactory, U32 value)
    {
        addWrongIntegerValue(exprFactory, value);
    }

    void addRow(BitmapIdTableTypeFactory& tableFactory,
                U32 bitmapId,
                U32 value,
                bool isSet)
    {
        EnumerationBitmapMapTypeFactory row;
        row.create(bitmapId, isSet, value);
        tableFactory.addRow(row.getDdh(), row.getSize());
    }

    void addTableToExpression(ExpressionTypeFactory& exprFactory,
                              BitmapIdTableTypeFactory& tableFactory)
    {
        ExpressionTermTypeFactory tableTerm;
        tableTerm.createBitmapIdTableExprTerm(tableFactory.getDdh(), tableFactory.getSize());

        exprFactory.addExprTerm(tableTerm.getDdh(), tableTerm.getSize());
    }

private:
    void addIntegerValue(ExpressionTypeFactory& exprFactory, U32 value)
    {
        ExpressionTermTypeFactory valueFactory;
        valueFactory.createIntegerExprTerm(value);

        exprFactory.addExprTerm(valueFactory.getDdh(), valueFactory.getSize());
    }

    void addWrongIntegerValue(ExpressionTypeFactory& exprFactory, U32 value)
    {
        ExpressionTermTypeFactory valueFactory;
        valueFactory.createWrongExprTerm(value);

        exprFactory.addExprTerm(valueFactory.getDdh(), valueFactory.getSize());
    }
};

TEST_F(ExpressionItemAtOpFixture, ItemAtTest)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 2U);

    addKeyValue(exprFactory, 39U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(2U);

    addRow(tableFactory, 23U, 29U, true);
    addRow(tableFactory, 33U, 39U, true);

    addTableToExpression(exprFactory, tableFactory);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    const psc::Number expectedValue(33U, psc::DATATYPE_INTEGER);
    const psc::DataStatus expectedStatus = psc::DataStatus::VALID;
    psc::Number actualValue;

    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedValue, actualValue);
}

TEST_F(ExpressionItemAtOpFixture, ItemAtUseDefaultValueTest)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 3U);

    addKeyValue(exprFactory, 40U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(2U);

    addRow(tableFactory, 23U, 29U, true);
    addRow(tableFactory, 33U, 39U, true);

    addTableToExpression(exprFactory, tableFactory);

    addDefaultValue(exprFactory, 55);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    const psc::Number expectedValue(55U, psc::DATATYPE_INTEGER);
    const psc::DataStatus expectedStatus = psc::DataStatus::VALID;

    psc::Number actualValue;
    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedValue, actualValue);
}

TEST_F(ExpressionItemAtOpFixture, ItemAtWithoutDefaultValueTest)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 2U);

    addKeyValue(exprFactory, 40U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(2U);

    addRow(tableFactory, 23U, 29U, true);
    addRow(tableFactory, 33U, 39U, true);

    addTableToExpression(exprFactory, tableFactory);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    const psc::DataStatus expectedStatus = psc::DataStatus::INVALID;
    psc::Number actualValue;

    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
}

TEST_F(ExpressionItemAtOpFixture, ItemAtWithWrongDefaultValueTest)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 3U);

    addKeyValue(exprFactory, 40U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(2U);

    addRow(tableFactory, 23U, 29U, true);
    addRow(tableFactory, 33U, 39U, true);

    addTableToExpression(exprFactory, tableFactory);

    addWrongDefaultValue(exprFactory, 55);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    const psc::DataStatus expectedStatus = psc::DataStatus::INCONSISTENT;
    psc::Number actualValue;

    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
}

TEST_F(ExpressionItemAtOpFixture, ItemAtWithWrongFirstTermTest)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 3U);

    addWrongKeyValue(exprFactory, 39U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(2U);

    addRow(tableFactory, 23U, 29U, true);
    addRow(tableFactory, 33U, 39U, true);

    addTableToExpression(exprFactory, tableFactory);

    addDefaultValue(exprFactory, 55);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    psc::Number actualValue;
    const psc::DataStatus expectedStatus = psc::DataStatus::INCONSISTENT;

    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
}

TEST_F(ExpressionItemAtOpFixture, ItemAtWithEmptyBitmapTableTest1)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 2U);

    addKeyValue(exprFactory, 39U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(0U);

    addTableToExpression(exprFactory, tableFactory);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    psc::Number actualValue;
    const psc::DataStatus expectedStatus = psc::DataStatus::INVALID;

    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
}

TEST_F(ExpressionItemAtOpFixture, ItemAtWithNotSetValueTest)
{
    ExpressionTypeFactory exprFactory;
    exprFactory.createExpr(psc::EXPRESSION_OPERATOR_ITEM_AT, 2U);

    addKeyValue(exprFactory, 39U);

    BitmapIdTableTypeFactory tableFactory;
    tableFactory.createExpr(2U);

    addRow(tableFactory, 23U, 29U, false);
    addRow(tableFactory, 33U, 39U, false);

    addTableToExpression(exprFactory, tableFactory);

    m_termFactory.createExpressionExprTerm(exprFactory.getDdh(), exprFactory.getSize());

    psc::Number actualValue;
    const psc::DataStatus expectedStatus = psc::DataStatus::INCONSISTENT;

    psc::DataStatus actualStatus =
        psc::expressionoperators::itemAt(m_termFactory.getDdh()->GetExpression(),
                                                 &m_context,
                                                 actualValue);

    EXPECT_EQ(expectedStatus, actualStatus);
}

