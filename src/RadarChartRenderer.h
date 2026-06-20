#pragma once
#include <QQuickPaintedItem>
#include "RadarChartModel.h"

class RadarChartRenderer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(RadarChartModel* model READ model WRITE setModel NOTIFY modelChanged)
public:
    explicit RadarChartRenderer(QQuickItem *parent = nullptr);

    RadarChartModel* model() const;
    void setModel(RadarChartModel *model);

    void paint(QPainter *painter) override;

signals:
    void modelChanged();

private:
    RadarChartModel *m_model = nullptr;
};