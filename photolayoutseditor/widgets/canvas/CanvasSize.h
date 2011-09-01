#ifndef CANVASSIZE_H
#define CANVASSIZE_H

#include <QString>
#include <QMap>
#include <QSizeF>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasSizeDialog;

    class CanvasSize
    {
        public:

            enum ResolutionUnits
            {
                UnknownResolutionUnit,
                PixelsPerMilimeter,
                PixelsPerCentimeter,
                PixelsPerInch,
                PixelsPerPoint,
                PixelsPerPicas,
            };

            enum SizeUnits
            {
                UnknownSizeUnit,
                Pixels,
                Milimeters,
                Centimeters,
                Inches,
                Points,
                Picas,
            };

            static QList<qreal> resolutionUnitsFactors();
            static QList<QString> resolutionUnitsNames();
            static QList<ResolutionUnits> resolutionUnits();
            static qreal resolutionUnitFactor(ResolutionUnits unit);
            static qreal resolutionUnitFactor(QString unitName);
            static QString resolutionUnitName(ResolutionUnits unit);
            static QString resolutionUnitName(qreal factor);
            static ResolutionUnits resolutionUnit(qreal factor);
            static ResolutionUnits resolutionUnit(QString name);
            static qreal resolutionConvert(qreal value, ResolutionUnits from, ResolutionUnits to);

            static QList<qreal> sizeUnitsFactors();
            static QList<QString> sizeUnitsNames();
            static QList<SizeUnits> sizeUnits();
            static qreal sizeUnitFactor(SizeUnits unit);
            static qreal sizeUnitFactor(QString unitName);
            static QString sizeUnitName(SizeUnits unit);
            static QString sizeUnitName(qreal factor);
            static SizeUnits sizeUnit(qreal factor);
            static SizeUnits sizeUnit(QString name);
            static qreal sizeConvert(qreal value, SizeUnits from, SizeUnits to);

            static int toPixels(qreal value, qreal resolution, SizeUnits sUnit, ResolutionUnits rUnit);
            static qreal fromPixels(int pixels, qreal resolution, SizeUnits sUnit, ResolutionUnits rUnit);

            CanvasSize();
            CanvasSize(const QSizeF & size, SizeUnits sUnit, const QSizeF & resolution, ResolutionUnits rUnit);

            QSizeF size() const;
            void setSize(const QSizeF & size);
            QSizeF size(SizeUnits unit) const;
            SizeUnits sizeUnit() const;
            void setSizeUnit(SizeUnits unit);
            QSizeF resolution() const;
            QSizeF resolution(ResolutionUnits unit) const;
            void setResolution(const QSizeF & resolution);
            ResolutionUnits resolutionUnit() const;
            void setResolutionUnit(ResolutionUnits unit);
            bool isValid() const;

            bool operator ==(const CanvasSize & size) const;
            bool operator !=(const CanvasSize & size) const;

        private:

            static QMap<ResolutionUnits,qreal> resolution_factors;
            static QMap<ResolutionUnits,QString> resolution_names;

            static QMap<SizeUnits,qreal> size_factors;
            static QMap<SizeUnits,QString> size_names;

            static void prepare_maps();

            SizeUnits m_size_unit;
            QSizeF m_size;
            ResolutionUnits m_resolution_unit;
            QSizeF m_resolution;

        friend class CanvasSizeDialog;
    };
}

#endif // CANVASSIZE_H
