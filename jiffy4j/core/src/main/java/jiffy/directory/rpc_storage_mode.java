/**
 * Autogenerated by Thrift Compiler (0.12.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package jiffy.directory;


@javax.annotation.Generated(value = "Autogenerated by Thrift Compiler (0.12.0)")
public enum rpc_storage_mode implements org.apache.thrift.TEnum {
  rpc_in_memory(0),
  rpc_in_memory_grace(1),
  rpc_on_disk(2);

  private final int value;

  private rpc_storage_mode(int value) {
    this.value = value;
  }

  /**
   * Get the integer value of this enum value, as defined in the Thrift IDL.
   */
  public int getValue() {
    return value;
  }

  /**
   * Find a the enum type by its integer value, as defined in the Thrift IDL.
   * @return null if the value is not found.
   */
  @org.apache.thrift.annotation.Nullable
  public static rpc_storage_mode findByValue(int value) { 
    switch (value) {
      case 0:
        return rpc_in_memory;
      case 1:
        return rpc_in_memory_grace;
      case 2:
        return rpc_on_disk;
      default:
        return null;
    }
  }
}
