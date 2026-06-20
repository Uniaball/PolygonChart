#include "RadarChartRenderer.h"
#include <QPainter>
#include <QPointF>
#include <QtMath>
#include <algorithm>
#include <numbers>

RadarChartRenderer::RadarChartRenderer(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setFillColor(Qt::white);
}

RadarChartModel* RadarChartRenderer::model() const { return m_model; }

void RadarChartRenderer::setModel(RadarChartModel *model)
{
    if (m_model == model) return;
    if (m_model)
        disconnect(m_model, &RadarChartModel::dataChanged, this, &QQuickItem::update);
    m_model = model;
    if (m_model)
        connect(m_model, &RadarChartModel::dataChanged, this, &QQuickItem::update);
    emit modelChanged();
    update();
}

void RadarChartRenderer::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (!m_model) return;
    int n = m_model->count();
    if (n < 3) return;

    QStringList names = m_model->names();
    std::vector<double> scores(n), maxScores(n);
    for (int i = 0; i < n; ++i) {
        scores[i] = m_model->scoreAt(i);
        maxScores[i] = m_model->maxScoreAt(i);
    }

    const double side = std::min(width(), height()) * 0.8;
    const QPointF center(width() / 2.0, height() / 2.0);
    const double radius = side / 2.0;
    const double angleStep = 2.0 * std::numbers::pi / n;

    auto indices = std::views::iota(0, n);

    painter->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    for (int level : {25, 50, 75, 100}) {
        QVector<QPointF> points;
        for (int i : indices) {
            double angle = -std::numbers::pi / 2.0 + i * angleStep;
            double r = radius * level / 100.0;
            points << center + QPointF(r * std::cos(angle), r * std::sin(angle));
        }
        painter->drawPolygon(points.data(), points.size());
    }

    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setPointSize(9);
    painter->setFont(font);

    for (int i : indices) {
        double angle = -std::numbers::pi / 2.0 + i * angleStep;
        QPointF outer = center + QPointF(radius * std::cos(angle), radius * std::sin(angle));
        painter->drawLine(center, outer);

        QPointF labelPos = center + QPointF((radius + 35) * std::cos(angle),
                                            (radius + 35) * std::sin(angle));
        QRectF textRect(labelPos.x() - 40, labelPos.y() - 12, 80, 24);
        QString label = (i < names.size()) ? names[i] : QString::number(i+1);
        painter->drawText(textRect, Qt::AlignCenter, label);
    }

    QVector<QPointF> scorePoints;
    for (int i : indices) {
        double angle = -std::numbers::pi / 2.0 + i * angleStep;
        double ratio = (maxScores[i] > 0) ? (scores[i] / maxScores[i]) : 0.0;
        double r = radius * ratio;
        scorePoints << center + QPointF(r * std::cos(angle), r * std::sin(angle));
    }

    painter->setBrush(QColor(100, 149, 237, 128));
    painter->setPen(QPen(Qt::blue, 2));
    painter->drawPolygon(QPolygonF(scorePoints));

    painter->setBrush(Qt::blue);
    painter->setPen(Qt::NoPen);
    for (int i = 0; i < scorePoints.size(); ++i) {
        painter->drawEllipse(scorePoints[i], 4, 4);

        double angle = -std::numbers::pi / 2.0 + i * angleStep;
        QPointF textPos = scorePoints[i] - QPointF(15 * std::cos(angle), 15 * std::sin(angle));
        QString scoreText = QString::number(scores[i], 'f', 1);

        painter->setPen(Qt::darkBlue);
        QFont smallFont = font;
        smallFont.setPointSize(8);
        painter->setFont(smallFont);
        painter->drawText(QRectF(textPos.x()-20, textPos.y()-10, 40, 20),
                          Qt::AlignCenter, scoreText);
        painter->setPen(Qt::NoPen);
    }
}