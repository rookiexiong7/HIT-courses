/**
 * @author See Contributors.txt for code contributors and overview of BadgerDB.
 *
 * @section LICENSE
 * Copyright (c) 2012 Database Group, Computer Sciences Department, University of Wisconsin-Madison.
 */

#pragma once

namespace badgerdb {

/**
 * @brief Identifier for a page in a file.
 */
typedef std::uint32_t PageId;

/**
 * @brief Identifier for a slot in a page.
 */
typedef std::uint16_t SlotId;

/**
 * @brief Identifier for a frame in buffer pool.
 */
typedef std::uint32_t FrameId;

/**
 * @brief Identifier for a record in a page.
 */
struct RecordId {
  /**
   * Number of page containing this record.
   */
  PageId page_number;

  /**
   * Number of slot within the page containing this record.
   */
  SlotId slot_number;

  /**
   * Returns true if this record ID refers to the same record as the given ID.
   *
   * @param rhs   Record ID to compare against.
   * @return  Whether the other ID refers to the same record as this one.
   */
  bool operator==(const RecordId& rhs) const {
    return page_number == rhs.page_number && slot_number == rhs.slot_number;
  }

  /**
   * Returns true if this record ID is different from the record as the given ID.
   *
   * @param rhs   Record ID to compare against.
   * @return  Whether the other ID is different from record as this one.
   */
  bool operator!=(const RecordId& rhs) const {
    return (page_number != rhs.page_number) || (slot_number != rhs.slot_number);
  }
};

}
