#include "RadarChartModel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RadarChartModel::RadarChartModel(QObject *parent) : QObject(parent)
{
    QStringList defaultNames;
    std::vector<double> defaultScores, defaultMax;
    for (int i = 1; i <= 5; ++i) {
        defaultNames << QString("项目%1").arg(i);
        defaultScores.push_back(100.0);
        defaultMax.push_back(100.0);
    }
    setData(defaultNames, defaultScores, defaultMax);
}

int RadarChartModel::count() const { return m_names.size(); }
QStringList RadarChartModel::names() const { return m_names; }

double RadarChartModel::scoreAt(int index) const
{
    if (index < 0 || index >= (int)m_scores.size()) return 0.0;
    return m_scores[index];
}

double RadarChartModel::maxScoreAt(int index) const
{
    if (index < 0 || index >= (int)m_maxScores.size()) return 100.0;
    return m_maxScores[index];
}

void RadarChartModel::setData(const QStringList &names,
                              const std::vector<double> &scores,
                              const std::vector<double> &maxScores)
{
    if (names.size() != scores.size() || scores.size() != maxScores.size())
        return;
    m_names = names;
    m_scores = scores;
    m_maxScores = maxScores;
    emit dataChanged();
}

void RadarChartModel::setScore(int index, double value)
{
    if (index < 0 || index >= (int)m_scores.size()) return;
    if (qFuzzyCompare(m_scores[index], value)) return;
    m_scores[index] = value;
    emit dataChanged();
}

void RadarChartModel::setMaxScore(int index, double value)
{
    if (index < 0 || index >= (int)m_maxScores.size()) return;
    if (qFuzzyCompare(m_maxScores[index], value)) return;
    m_maxScores[index] = value;
    emit dataChanged();
}

void RadarChartModel::setName(int index, const QString &name)
{
    if (index < 0 || index >= (int)m_names.size()) return;
    if (m_names[index] == name) return;
    m_names[index] = name;
    emit dataChanged();
}

bool RadarChartModel::saveToJson(const QString &filePath)
{
    QJsonArray items;
    for (int i = 0; i < count(); ++i) {
        QJsonObject obj;
        obj["name"] = m_names[i];
        obj["score"] = m_scores[i];
        obj["maxScore"] = m_maxScores[i];
        items.append(obj);
    }
    QJsonDocument doc(items);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool RadarChartModel::loadFromJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !doc.isArray())
        return false;

    QJsonArray items = doc.array();
    int n = items.size();
    if (n < 3 || n > 12) return false;

    QStringList names;
    std::vector<double> scores, maxScores;
    names.reserve(n);
    scores.reserve(n);
    maxScores.reserve(n);

    for (int i = 0; i < n; ++i) {
        QJsonObject obj = items[i].toObject();
        names << obj.value("name").toString(QString("项目%1").arg(i+1));
        scores.push_back(obj.value("score").toDouble(0.0));
        maxScores.push_back(obj.value("maxScore").toDouble(100.0));
    }
    setData(names, scores, maxScores);
    return true;
}