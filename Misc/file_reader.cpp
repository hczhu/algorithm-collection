#include "common_header_zhc.h"
#include <algorithm>
#include <new>
#include <stdint.h>

class file_line_reader_t {
  char *m_current_buffer;
  uint32_t m_buffer_size;
  FILE *m_file;
  uint32_t m_pos;
  int m_fileno;
  static const char split = '\n';
  long long m_file_size, m_rem_block, m_offset;

public:
  file_line_reader_t()
      : m_current_buffer(NULL), m_file(NULL), m_pos(0), m_file_size(0),
        m_rem_block(0){};
  int create(uint32_t buffer_size, FILE *in_file) {
    if (in_file == NULL) {
      WRITE_LOG(UL_LOG_FATAL, "NULL file pointer");
      return -1;
    }
    m_file = in_file;
    m_offset = 0;

    struct stat st;
    if (fstat(fileno(m_file), &st)) {
      WRITE_LOG(UL_LOG_FATAL, "fstat failed");
      return -1;
    }
    m_fileno = fileno(in_file);
    m_file_size = st.st_size;
    m_buffer_size = std::min(m_file_size, (long long)buffer_size);
    try {
      m_current_buffer = new char[m_buffer_size + 1];
    } catch (std::bad_alloc) {
      WRITE_LOG(UL_LOG_FATAL, "new buffer failed");
      return -1;
    }
    m_rem_block = m_file_size / m_buffer_size;
    m_pos = m_buffer_size;
    m_current_buffer[m_pos] = split;
    if (m_file_size % m_buffer_size) {
      uint32_t read_byte = (m_file_size % m_buffer_size);
      if (read_byte != pread(m_fileno,
                             m_current_buffer + m_buffer_size - read_byte,
                             read_byte, 0)) {
        WRITE_LOG(UL_LOG_FATAL, "pread failed");
        return -1;
      }
      m_offset += read_byte;
      m_pos -= read_byte;
    }
    return 0;
  }
  ~file_line_reader_t() {
    if (NULL != m_current_buffer) {
      delete[] m_current_buffer;
    }
  }
  int get_line(char *out_buffer, uint32_t out_buffer_size) {
    const int start = m_pos;
    while (split != m_current_buffer[m_pos]) {
      m_pos++;
    }
    if (m_pos < m_buffer_size) {
      int ret = m_pos - start;
      int len = std::min((int)out_buffer_size - 1, ret);
      memcpy(out_buffer, m_current_buffer + start, len);
      out_buffer[len] = 0;
      m_pos++;
      return ret;
    }
    int ret = std::min(out_buffer_size - 1, m_pos - start);
    memcpy(out_buffer, m_current_buffer + start, ret);
    m_pos = 0;
    if (m_rem_block == 0) {
      out_buffer[ret] = 0;
      return ret == 0 ? -1 : ret;
    }
    if (m_buffer_size !=
        pread(m_fileno, m_current_buffer, m_buffer_size, m_offset)) {
      WRITE_LOG(UL_LOG_FATAL, "pread failed");
      return -2;
    }
    m_rem_block--;
    m_offset += m_buffer_size;
    m_current_buffer[m_buffer_size] = split;
    while (split != m_current_buffer[m_pos]) {
      m_pos++;
    }
    if (m_pos == m_buffer_size) {
      WRITE_LOG(UL_LOG_FATAL,
                "The inner buffer size [%u] is shorter than current line size.",
                m_buffer_size);
      return -2;
    }
    int len = std::min(m_pos, out_buffer_size - 1 - ret);
    memcpy(out_buffer + ret, m_current_buffer, len);
    ret += len;
    out_buffer[ret] = 0;
    m_pos++;
    return ret;
  }
};
