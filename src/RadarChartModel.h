#pragma once
#include <QObject>
#include <QStringList>
#include <vector>

class RadarChartModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY dataChanged)
    Q_PROPERTY(QStringList names READ names NOTIFY dataChanged)
public:
    explicit RadarChartModel(QObject *parent = nullptr);

    int count() const;
    QStringList names() const;
    Q_INVOKABLE double scoreAt(int index) const;
    Q_INVOKABLE double maxScoreAt(int index) const;

    Q_INVOKABLE void setData(const QStringList &names,
                             const std::vector<double> &scores,
                             const std::vector<double> &maxScores);

    Q_INVOKABLE void setScore(int index, double value);
    Q_INVOKABLE void setMaxScore(int index, double value);
    Q_INVOKABLE void setName(int index, const QString &name);

    Q_INVOKABLE bool saveToJson(const QString &filePath);
    Q_INVOKABLE bool loadFromJson(const QString &filePath);

signals:
    void dataChanged();

private:
    QStringList m_names;
    std::vector<double> m_scores;
    std::vector<double> m_maxScores;
};