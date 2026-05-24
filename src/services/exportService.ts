import { WaterLog } from './firestoreService'
import { formatPhilippinePeso } from '@/utils/currency'

export class ExportService {
  // Export to CSV
  exportToCSV(logs: WaterLog[], filename = 'water-sales.csv'): void {
    const headers = ['Timestamp', 'Water Type', 'Amount', 'Timing', 'Date']
    const rows = logs.map(log => [
      new Date(log.timestamp).toLocaleString(),
      log.isCold ? 'Cold' : 'Regular',
      formatPhilippinePeso(log.amount),
      log.timeSynced === false ? 'Approximate flush time' : 'Synced',
      new Date(log.timestamp).toISOString().split('T')[0]
    ])

    const csvContent = [
      headers.join(','),
      ...rows.map(row => row.map(cell => `"${cell}"`).join(','))
    ].join('\n')

    const blob = new Blob([csvContent], { type: 'text/csv' })
    const url = window.URL.createObjectURL(blob)
    const link = document.createElement('a')
    link.href = url
    link.download = filename
    link.click()
    window.URL.revokeObjectURL(url)
  }

  // Export to PDF
  async exportToPDF(logs: WaterLog[], filename = 'water-sales.pdf'): Promise<void> {
    const { default: jsPDF } = await import('jspdf')
    const doc = new jsPDF()
    doc.setFontSize(16)
    doc.text('Water Sales Report', 10, 10)

    doc.setFontSize(10)
    const tableData = logs.map(log => [
      new Date(log.timestamp).toLocaleString(),
      log.isCold ? 'Cold' : 'Regular',
      formatPhilippinePeso(log.amount),
      log.timeSynced === false ? 'Approximate' : 'Synced'
    ])

    // Create table using autoTable plugin (if available) or manual
    let y = 30
    const headers = ['Timestamp', 'Type', 'Amount', 'Timing']

    doc.setFont(undefined, 'bold')
    headers.forEach((header, i) => {
      doc.text(header, 10 + i * 60, y)
    })

    doc.setFont(undefined, 'normal')
    y += 10

    tableData.forEach((row, idx) => {
      if (y > 270) {
        doc.addPage()
        y = 10
      }
      row.forEach((cell, i) => {
        doc.text(cell, 10 + i * 60, y)
      })
      y += 8
    })

    doc.save(filename)
  }
}

export default new ExportService()
