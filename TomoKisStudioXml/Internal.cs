using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace TomoKisStudioXml
{
    static class Internal
    {
        /// <summary>
        /// Pobierz scieżke i nazwę pliku XML na podstawie nazwy pliku wykonywalnego
        /// </summary>
        /// <returns>Pełna scieżka do pliku</returns>
        public static string GetStorePath()
        {
            return GetStorePath(null);
        }

        /// <summary>
        /// Pobierz scieżkę i nazwę pliku XML na podstawie nazwy przekazanej w parametrze
        /// </summary>
        /// <param name="xml_file_name">Nazwa pliku z rozszerzeniem XML</param>
        /// <returns>Pełna scieżka do pliku</returns>
        public static string GetStorePath(string xml_file_name)
        {
            string path = System.Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            path = Path.Combine(path, "Tomo KIS Studio");

            if (string.IsNullOrEmpty(xml_file_name))
                path = Path.Combine(path, Path.ChangeExtension(Path.GetFileName(Application.ExecutablePath), "xml"));
            else
                path = Path.Combine(path, xml_file_name);

            return path;
        }
    }
}
