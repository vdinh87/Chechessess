// Enhanced styling for the ability description panel
// Replace the existing methods in your code with these enhanced versions

// Add implementation for the ability description update methods
void MainWindow::updateAbilityDescription(const QString &text, bool isHtml)
{
    if (ui && ui->label_67)
    {
        // Apply a stylish background with border radius and gradient
        ui->label_67->setStyleSheet(
            "QLabel {"
            "  font-family: 'Book Antiqua', 'Palatino Linotype', serif;"
            "  font-size: 11pt;"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2e2d2c, stop:1 #1f1e1d);"
            "  color: #e8e6e3;"
            "  border: 2px solid #3a3937;"
            "  border-radius: 10px;"
            "  padding: 14px;"
            "  margin: 5px;"
            "}"
            "QLabel a { color: #c19a49; }" // Gold color for any links or special elements
        );

        // Create a custom styled HTML wrapper for the content
        QString headerStyle = "display: block; text-align: center; font-family: 'Book Antiqua', 'Palatino Linotype', serif; "
                              "font-size: 15pt; font-weight: bold; color: #c19a49; margin-bottom: 10px; "
                              "border-bottom: 1px solid #c19a49; padding-bottom: 5px; letter-spacing: 1px;";

        QString containerStyle = "padding: 8px; background-color: rgba(30, 30, 28, 0.7); border-radius: 5px;";

        // Apply custom HTML styling to the content with medieval/fantasy styling
        QString styledText = text;
        if (isHtml)
        {
            // Create a decorated header
            styledText = "<div style='" + headerStyle + "'>✧ ABILITIES ✧</div>" +
                         "<div style='" + containerStyle + "'>";

            // Replace the <b> tags with styled headers
            QString processedText = text;
            processedText.replace("<b>", "<div style='color: #c19a49; font-size: 13pt; font-weight: bold; margin-top: 10px; text-decoration: underline;'>");
            processedText.replace("</b>", "</div>");

            // Style the bullet points for abilities
            processedText.replace("• ", "<div style='margin: 5px 0;'><span style='color: #c19a49; font-weight: bold;'>❖</span> <span style='color: #b8c8e0; font-weight: bold;'>");
            processedText.replace("<br>•", "</span></div><div style='margin: 5px 0;'><span style='color: #c19a49; font-weight: bold;'>❖</span> <span style='color: #b8c8e0; font-weight: bold;'>");

            // Close any unclosed spans or divs
            if (processedText.count("<span") > processedText.count("</span>"))
            {
                processedText += "</span>";
            }
            if (processedText.count("<div") > processedText.count("</div>"))
            {
                processedText += "</div>";
            }

            styledText += processedText + "</div>";

            // Add a decorative footer
            styledText += "<div style='text-align: center; margin-top: 10px; color: #c19a49;'>⚜ ⚜ ⚜</div>";
        }

        ui->label_67->setText(styledText);
        ui->label_67->setTextFormat(Qt::RichText); // Always use rich text for styled content
        ui->label_67->setWordWrap(true);
        ui->label_67->setMinimumHeight(350); // More height for the enhanced styling
        ui->label_67->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    }
}

void MainWindow::resetAbilityDescription()
{
    if (ui && ui->label_67)
    {
        // Apply the same styled background but with a default message
        ui->label_67->setStyleSheet(
            "QLabel {"
            "  font-family: 'Book Antiqua', 'Palatino Linotype', serif;"
            "  font-size: 11pt;"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2e2d2c, stop:1 #1f1e1d);"
            "  color: #e8e6e3;"
            "  border: 2px solid #3a3937;"
            "  border-radius: 10px;"
            "  padding: 14px;"
            "  margin: 5px;"
            "}");

        // Create a decorative HTML layout for the default state
        QString headerStyle = "display: block; text-align: center; font-family: 'Book Antiqua', 'Palatino Linotype', serif; "
                              "font-size: 15pt; font-weight: bold; color: #c19a49; margin-bottom: 10px; "
                              "border-bottom: 1px solid #c19a49; padding-bottom: 5px; letter-spacing: 1px;";

        QString messageStyle = "display: block; text-align: center; color: #888888; font-style: italic; margin-top: 20px;";

        QString decorStyle = "display: block; text-align: center; color: #c19a49; font-size: 20pt; margin: 30px 0;";

        // Assemble the styled content
        QString content =
            "<div style='" + headerStyle + "'>✧ ABILITIES ✧</div>" +
            "<div style='" + messageStyle + "'>Capture a piece to see special abilities</div>" +
            "<div style='" + decorStyle + "'>⚔</div>" +
            "<div style='" + messageStyle + "'>Each piece has unique powers in chess-in-chess battles</div>" +
            "<div style='text-align: center; margin-top: 30px; color: #c19a49;'>⚜ ⚜ ⚜</div>";

        ui->label_67->setText(content);
        ui->label_67->setTextFormat(Qt::RichText);
        ui->label_67->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    }
}