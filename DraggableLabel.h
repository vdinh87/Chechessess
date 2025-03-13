#ifndef DraggableLabel_H
#define DraggableLabel_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <vector>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>

// Forward declaration to avoid circular dependency
class MainWindow;

class DraggableLabel : public QLabel
{
    Q_OBJECT
public:
    // Declare constructors but don't implement them in header - now in CPP file
    DraggableLabel();
    DraggableLabel(QWidget *parent = 0);

    // Function declarations - implementations now in CPP file
    void setDraggable(bool can_drag);
    void setMainWindow(MainWindow *window);

    // Remove this method as it should be global or in MainWindow
    /*
    void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel *> &draggableLabels)
    {
        for (int i = 0; i < 64; i++)
        {
            DraggableLabel *label = draggableLabels[i];
            bool bit = (bitboard & (1ULL << i));
            if (bit)
            {
                // Save current style
                QString currentStyle = label->styleSheet();
                if (!currentStyle.contains("background-color"))
                {
                    label->setProperty("originalStyle", currentStyle);
                }

                // Add highlight
                currentStyle += " background-color: rgba(0, 255, 0, 0.3);";
                label->setStyleSheet(currentStyle);
            }
        }
    }
    */

protected:
    bool draggable = false;
    QPoint dragStartPosition;
    QString originalStyle;
    MainWindow *mainWindow = nullptr;

    // Method declarations - implementations now in CPP file
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void dragStarted(DraggableLabel *source);
    void dragEntered(DraggableLabel *target);
    void dropOccurred(DraggableLabel *source, DraggableLabel *target);
};

#endif // DraggableLabel_H
